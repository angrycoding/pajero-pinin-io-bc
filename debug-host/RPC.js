var SerialPort = require('serialport');

var EventEmitter = require('events').EventEmitter;

const WAIT_INTERVAL = 250;

const RPC_KEY = 0;
const RPC_TYPE = 1;
const RPC_CRC = 2;

const RPC_NULL = 'N'.charCodeAt(0);
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

	if (!(this instanceof RPC))
		return new RPC(comName, baudRate);

	this.buffer = [];
	this.state = RPC_KEY;
	this.rxBuffer = [];

	var serialPort = new SerialPort(comName, {
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
	this.serialPort.close(ret);
};

RPC.prototype.writeNull = function(key) {
	var buffer = [key, RPC_NULL];
	buffer.push(iso_checksum(buffer));
	this.serialPort.write(buffer);
};

RPC.prototype.processIncoming = function(data) {

	var available,
		buffer = this.buffer,
		rxBuffer = this.rxBuffer;

	Array.prototype.push.apply(buffer, data);

	loop: while (available = buffer.length) switch (this.state) {

		// читаем ключ
		case RPC_KEY: {
			rxBuffer.splice(0, Infinity, buffer.shift());
			this.state = RPC_TYPE;
			break;
		}

		// читаем тип значения
		case RPC_TYPE: {
			switch (rxBuffer.push(buffer[0]), buffer.shift()) {
				case RPC_NULL: this.state = RPC_CRC; break;
				case RPC_UINT8: this.state = RPC_UINT8; break;
				case RPC_FLOAT: this.state = RPC_FLOAT; break;
				case RPC_UINT32: this.state = RPC_UINT32; break;
				default: this.state = RPC_KEY; break;
			}
			break;
		}

		case RPC_UINT8: {
			rxBuffer.push(buffer.shift());
			this.state = RPC_CRC;
			break;
		}

		case RPC_FLOAT:
		case RPC_UINT32: {
			if (available < 4) break loop;
			rxBuffer.push(buffer.shift());
			rxBuffer.push(buffer.shift());
			rxBuffer.push(buffer.shift());
			rxBuffer.push(buffer.shift());
			this.state = RPC_CRC;
			break;
		}


		// проверяем контрольную сумму
		case RPC_CRC: {
			this.state = RPC_KEY;
			if (buffer.shift() == iso_checksum(rxBuffer)) switch (rxBuffer[1]) {
				case RPC_NULL: this.emit('pid', rxBuffer[0], null); break;
				case RPC_UINT8: this.emit('pid', rxBuffer[0], new Buffer([rxBuffer[2]]).readUInt8(0)); break;
				case RPC_FLOAT: this.emit('pid', rxBuffer[0], new Buffer([rxBuffer[2], rxBuffer[3], rxBuffer[4], rxBuffer[5]]).readFloatLE(0)); break;
				case RPC_UINT32: this.emit('pid', rxBuffer[0], new Buffer([rxBuffer[2], rxBuffer[3], rxBuffer[4], rxBuffer[5]]).readUInt32LE(0)); break;
			}
			break;
		}

	}
};




module.exports = RPC;