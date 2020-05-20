from flask import Flask, render_template
from flask_socketio import SocketIO

files = {}
for fname in ['page.html', 'socket.io.js', 'handler.js', \
              'bridge_page.html', 'bridge_handler.js']:
    f = open(fname)
    files[fname] = f.read()
    f.close()

app = Flask(__name__)
sio = SocketIO(app, static_url_path='')

connections = 0
sio_conn = 0

qsize = 0
@app.route('/test')
def hello():
    return files['page.html']
@app.route('/')
def bridge():
    return files['bridge_page.html']

@app.route('/socket.io.js')
def sio_js():
    return files['socket.io.js']

@app.route('/handler.js')
def hdlr_js():
    return files['handler.js']

@app.route('/bridge_handler.js')
def bhdlr_js():
    return files['bridge_handler.js']

#@sio.on('message')
#def handle(s):
#    print('message: %s' % (s))
#    sio.send('repeat: '+s)

query_queue = []
@sio.on('query')
def handleq(data):
    global qsize, query_queue
    qsize += 1
    query_queue.append(data)
    print(query_queue)
    print('received query, emitting server_query')
    sio.emit('queue_pos', {'qid': data['qid'], 'pos': qsize})
    sio.emit('server_query', data, broadcast=True)

@sio.on('get_queue_pos')
def get_qsize(data):
    k = 0
    while k < len(query_queue) and query_queue[k]['qid'] != data['qid']:
        k += 1
    if k == len(query_queue):
        k = -1
    else:
        k += 1
    sio.emit('queue_pos', {'qid': data['qid'], 'pos': k})
    
@sio.on('server_result')
def send_back(data):
    global qsize, query_queue
    query_queue = query_queue[1:]
    qsize -= 1
    print('received server_result, emitting result')
    sio.emit('result', data, broadcast=True)

@sio.on('ack')
def handle_ack(data):
    print('received ack, emitting server_connect')
    sio.emit('server_connect', data, broadcast=True)

nservers = 0
@sio.on('server_ready')
def handle_ready(data):
    global nservers
    print('new server ready')
    nservers += 1

@sio.on('server_donezo')
def handle_donezo(data):
    global nservers
    print('server going offline')
    if nservers > 0:
        nservers -= 1

@sio.on('client_ready')
def client_ready(data):
    sio.emit('server_count', {'count': nservers})
