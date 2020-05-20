import socketio, subprocess

addr = 'http://127.0.0.1:5000/'
# addr = 'http://vietafan.pythonanywhere.com/'
# addr = 'http://bridge-sdds.herokuapp.com/'

sio = socketio.Client()
@sio.on('connect')
def svr_ready():
    sio.emit('server_ready', {})

@sio.on('disconnect')
def svr_donezo():
    sio.emit('server_donezo', {})

def parse_results(raw_str):
    print(raw_str)
    piece1, piece2, piece3 = raw_str[5:].split('play')
    piece2 = 'play'+piece2
    piece3 = 'play'+piece3
    trick_counts = {}
    for line in piece1.split('\\n')[1:-1]:
        vals = line.split(',')
        trick_counts[vals[0]] = list(map(int, vals[1:]))
    lines2 = piece2.split('\\n')
    plays = lines2[0].split(',')[1:]
    mat = [line.split(',')[1:] for line in lines2[1:-1]]
    lines3 = piece3.split('\\n')
    sdmat = [line.split(',')[1:] for line in lines3[1:-1]]
    return plays, trick_counts, mat, sdmat

def get_trick_mat(tcts,plays,ordering):
    tmin = 0
    tmax = 13
    sums = [sum([tcts[x][n] for x in tcts]) for n in range(14)]
    while sums[tmin] == 0:
        tmin += 1
    while sums[tmax] == 0:
        tmax -= 1
    html = '<table><style> td, th {border: 1px solid #000000;}</style><tr><td>Play \ Tricks</td>'
    for k in range(tmin,tmax+1):
        html += '<td>%s</td>' % (k)
    html += '</tr>'
    for i in ordering:
        x = plays[i]
        html += '<tr><td>%s</td>' % (x)
        for k in range(tmin,tmax+1):
            html += '<td>%s</td>' % (tcts[x][k])
        html += '</tr>'
    html += '</table>'
    return html

def get_imp_mat(plays,imat, ordering,label='avg IMPs'):
    html = '<table><style> td, th {border: 1px solid #000000;}</style><tr><td>%s</td>' % (label)
    n = len(plays)
    for k in ordering:
        html += '<td>%s</td>' % (plays[k])
    html += '</tr>'
    for i in ordering:
        html += '<tr><td>%s</td>' % (plays[i])
        for j in ordering:
            if imat[i][j] == '-0':
                imat[i][j] = '0'
            html += '<td>%s</td>' % (imat[i][j])
        html += '</tr>'
    html += '</table>'
    return html

def get_ordering(imat):
    return [x[1] for x in sorted([(sum(map(float,imat[j])),j) for j in range(len(imat))],key=lambda x: -x[0])]    

@sio.on('server_query')
def handle(data):
    sio.emit('ack', {'qid': data['qid']})
    print('handling query of id %s' % (data['qid']))
    print('deal state:\n%s' % (data['deal_state']))
    print('constraints:\n%s' % (data['constraints']))
    output = str(subprocess.run(['/home/vietafan/Sams_folder/cpp_files/sdds/main'], stdout=subprocess.PIPE, input=bytes('%s\ndone\n%s\ndone\n%s\n' % (data['deal_state'], data['constraints'], data['nboards']), encoding='utf-8')).stdout)[2:-1]
    print('output: %s' % (output))
    plays, tcts, impmat, impsdmat = parse_results(output)
    ordering = get_ordering(impmat)
    full_output = '<p>frequencies of winning each number of tricks, by play</p>'+get_trick_mat(tcts, plays, ordering)+'<p>average IMPs won by play at left vs. play on top</p>'+get_imp_mat(plays, impmat, ordering)+'<p>standard deviation of sample average IMPs (left play vs. top play)</p><p>(note: this SD will not be accurate if it\'s comparing plays of equivalent cards; in this case, the real SD is naturally zero)</p>'+get_imp_mat(plays, impsdmat, ordering, label='SD of sample avg')
    sio.emit('server_result', {'qid': data['qid'], 'raw_output': full_output})

sio.connect(addr)
