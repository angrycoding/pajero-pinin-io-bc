var RPC = require('./RPC');
var rpc = new RPC('/dev/tty.usbserial-A921PBNR', 500000);
var io = require('socket.io')(9999);

rpc.on('pid', function(key, value) {
	io.sockets.emit('pid', key, isFinite(value) ? value : 'INF');
});