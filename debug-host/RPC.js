var SerialPort = require('serialport');

var EventEmitter = require('events').EventEmitter;

const WAIT_INTERVAL = 250;

const RPC_START = 0;
const RPC_KEY = 1;
const RPC_CHECK = 2;

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
		this.testBuff = [];

		var serialPort = this.sp = new SerialPort(comName, {
			baudRate: baudRate,
			autoOpen: false
		});

		serialPort.on('error', function() {
			// console.info('ERR', arguments)
		});

		serialPort.on('open', function() {
			// console.info('PORT_OPEN', arguments)
		});
		serialPort.on('data', (data) => this.processIncoming(data));
		serialPort.on('close', function() {
			// console.info('PORT_CLOSE', arguments)
		});

		waitConnection(this.serialPort = serialPort);


	} else return new RPC(comName, baudRate);
}

RPC.prototype = Object.create(EventEmitter.prototype);


function iso_checksum(data) {
	var crc = 0;
	for(var i=0; i<data.length; i++) {
		crc = crc + data[i];
		if (crc > 255)
			crc = crc  - 256;
	}

  return crc;
}

RPC.prototype.exit = function(ret) {
	this.sp.close(ret);
};

RPC.prototype.processIncoming = function(data) {

	var available, buffer = this.buffer;

	Array.prototype.push.apply(buffer, data);

	loop: while (available = buffer.length) switch (this.state) {

		case RPC_START:
			this.testBuff = [buffer[0]];
			switch (buffer.shift()) {
				case RPC_UINT8: this.state = RPC_UINT8; break;
				case RPC_UINT32: this.state = RPC_UINT32; break;
				case RPC_FLOAT: this.state = RPC_FLOAT; break;
			}
			break;

		case RPC_UINT8:
			Array.prototype.push.apply(this.testBuff, buffer.slice(0, 1));
			this.responseValue = new Buffer(buffer.splice(0, 1)).readUInt8(0);
			this.state = RPC_KEY;
			break;

		case RPC_UINT32:
			if (available < 4) break loop;
			Array.prototype.push.apply(this.testBuff, buffer.slice(0, 4));
			this.responseValue = new Buffer(buffer.splice(0, 4)).readUInt32LE(0);
			this.state = RPC_KEY;
			break;

		case RPC_FLOAT: {
			if (available < 4) break loop;
			Array.prototype.push.apply(this.testBuff, buffer.slice(0, 4));
			this.responseValue = new Buffer(buffer.splice(0, 4)).readFloatLE(0);
			this.state = RPC_KEY;
			break;
		}

		case RPC_KEY:
			this.responseKey = buffer.shift();
			this.testBuff.push(this.responseKey);
			this.state = RPC_CHECK;
			break;

		case RPC_CHECK:
			var a = buffer.shift();
			var b  = iso_checksum(this.testBuff);
		// console.info(this.testBuff, a, b)

			if (a === b) {
				// console.info('good', this.responseKey, this.responseValue)
				this.emit('pid', this.responseKey, this.responseValue);
			} else {
				// console.info(a, b)
			}
			this.state = RPC_START;


	}


};




module.exports = RPC;