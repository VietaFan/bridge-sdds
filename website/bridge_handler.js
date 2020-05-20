var socket = io();
socket.on('connect', function(data) {
    console.log("connected to server");
    socket.emit('client_ready', {});
});

function submitQuery() {
    var dstate = document.getElementById("deal_state").value;
    var constr = document.getElementById("constraints").value;
    var nboards = Math.min(200, Number(document.getElementById("nboards").value));
    var id = Math.round(Math.random()*1000000000);
    document.getElementById("qid").value = id;
    console.log(id);
    document.getElementById("result").innerHTML = "";
    document.getElementById("status").innerHTML = "";
    document.getElementById("queue_pos").innerHTML = "";
    socket.emit('query', {'deal_state': dstate, 'constraints': constr, 'qid': id, 'nboards': nboards});
}

socket.on('server_connect', function(data) {
    console.log(data['qid']);
    console.log(document.getElementById("qid").value);
    if (Number(data['qid']) === Number(document.getElementById("qid").value)) {
        document.getElementById("status").innerHTML = "query received: running simulation";
    }
});

socket.on('result', function(data) {
    if (Number(data['qid']) === Number(document.getElementById("qid").value)) {
        document.getElementById("queue_pos").innerHTML = "";
        document.getElementById("status").innerHTML = "";
        document.getElementById("result").innerHTML = data['raw_output'];
    }
});

socket.on('server_count', function(data) {
    document.getElementById("server_status").innerHTML = "# double dummy servers available: "+data['count'];
});

socket.on('queue_pos', function(data) {
    if (Number(data['qid']) === Number(document.getElementById("qid").value)) {
        document.getElementById("queue_pos").innerHTML = "queue position: "+data['pos'];
    }
});
