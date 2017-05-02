var SerialPort = require('serialport');

var EventEmitter = require('events').EventEmitter;

const WAIT_INTERVAL = 250;

const RPC_START = 0;
const RPC_KEY = '#'.charCodeAt(0);
const RPC_VALUE = 1;

const RPC_BYTE = 'B'.charCodeAt(0);
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
			if (buffer.shift() === RPC_KEY) 
				this.state = RPC_KEY;
			break;

		case RPC_KEY:
			this.responseKey = buffer.shift();
			this.state = RPC_VALUE;
			break;

		case RPC_VALUE:
			switch (buffer.shift()) {
				case RPC_BYTE: this.state = RPC_BYTE; break;
				case RPC_FLOAT: this.state = RPC_FLOAT; break;
				default: this.state = RPC_START; break;
			}
			break;

		case RPC_BYTE:
			this.responseValue = new Buffer(buffer.splice(0, 1)).readUInt8(0);
			this.state = RPC_START;
			this.emit('pid', this.responseKey, this.responseValue);
			break;

		case RPC_FLOAT: {
			if (available < 4) break loop;
			this.responseValue = new Buffer(buffer.splice(0, 4)).readFloatLE(0);
			this.state = RPC_START;
			this.emit('pid', this.responseKey, this.responseValue);
			break;
		}

	}


};




module.exports = RPC;