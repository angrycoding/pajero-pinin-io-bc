var SerialPort = require('serialport');

var EventEmitter = require('events').EventEmitter;

const WAIT_INTERVAL = 250;

const RPC_START = 0;
const RPC_KEY = 1;

const RPC_UINT8 = 'B'.charCodeAt(0);
const RPC_UINT32 = 'L'.charCodeAt(0);
const RPC_FLOAT = 'F'.charCodeAt(0);

function waitConnection(serialPort) {
	serialPort.open(function(error) {
		if (error) setTimeout(
			waitConnection,
			WAIT_INTERVAL,
			serialPort
		);
	});
}

function RPC(comName, baudRate) {
	if (this instanceof RPC) {

		this.buffer = [];
		this.responseKey = null;
		this.responseValue = null;
		this.state = RPC_START;

		var serialPort = new SerialPort(comName, {
			baudRate: baudRate,
			autoOpen: false
		});

		serialPort.on('open', function() {});
		serialPort.on('data', (data) => this.processIncoming(data));
		serialPort.on('close', function() {});

		waitConnection(this.serialPort = serialPort);


	} else return new RPC(comName, baudRate);
}

RPC.prototype = Object.create(EventEmitter.prototype);


RPC.prototype.processIncoming = function(data) {

	var available, buffer = this.buffer;

	Array.prototype.push.apply(buffer, data);

	loop: while (available = buffer.length) switch (this.state) {

		case RPC_START:
			switch (buffer.shift()) {
				case RPC_UINT8: this.state = RPC_UINT8; break;
				case RPC_UINT32: this.state = RPC_UINT32; break;
				case RPC_FLOAT: this.state = RPC_FLOAT; break;
			}
			break;

		case RPC_UINT8:
			this.responseValue = new Buffer(buffer.splice(0, 1)).readUInt8(0);
			this.state = RPC_KEY;
			break;

		case RPC_UINT32:
			if (available < 4) break loop;
			this.responseValue = new Buffer(buffer.splice(0, 4)).readUInt32LE(0);
			this.state = RPC_KEY;
			break;

		case RPC_FLOAT: {
			if (available < 4) break loop;
			this.responseValue = new Buffer(buffer.splice(0, 4)).readFloatLE(0);
			this.state = RPC_KEY;
			break;
		}

		case RPC_KEY:
			this.emit('pid', buffer.shift(), this.responseValue);
			this.state = RPC_START;
			break;

	}


};




module.exports = RPC;