var RPC = require('./RPC');
var rpc = new RPC('/dev/tty-usbserial1', 115200);
var io = require('socket.io')(9999);

rpc.on('pid', function(key, value) {
	io.sockets.emit('pid', pid, value);
});