var RPC = require('./RPC');
var rpc = new RPC('/dev/tty.usbserial-A921PBNR', 230400);
var PID_REQUEST_INTERVAL = 10;

// clear screen, hide cursor
process.stdout.write('\033c\033[?25l');

function exitHandler(options, err) {
	process.stdout.write('\033[?25h\n');
	process.exit();
}

process.on('exit', exitHandler);
process.on('SIGINT', exitHandler);
process.on('uncaughtException', exitHandler);

var PIDS = {

	0x00: {
		display: [{
			label: 'Кондиционер включен',
			formula: function(value) {
				return !!(value & (1 << 4));
			}
		}, {
			label: 'Селектор P/N',
			formula: function(value) {
				return !!(value & (1 << 5));
			}
		}]
	},

	0x03: {
		display: {
			label: 'Педаль тормоза',
			formula: function(value) {
				return !!(value & (1 << 3));
			}
		}
	},

	0x10: {
		display: {
			label: 'Температура ОЖ',
			formula: function(value) {
				return (value - 40);
			}
		}
	},

	0x14: {
		display: {
			label: 'Заряд аккумулятора',
			formula: function(value) {
				return (0.07333 * value);
			}
		}
	},

	0x15: {
		display: {
			label: 'Барометр',
			formula: function(value) {
				return ((0.49 * value) * 7.50062);
			}
		}
	},

	0x17: {
		display: {
			label: 'Положение ДЗ',
			formula: function(value) {
				return (value * 100 / 255);
			}
		}
	},

	0x21: {
		display: {
			label: 'Обороты двигателя',
			formula: function(value) {
				return (value * 31.25);
			}
		}
	},

	0x2D: {
		display: {
			label: 'Скорость вентилятора печки',
			formula: function(value) {
				var index = [110, 111, 112, 113, 114].indexOf(value);
				return (index === -1 ? value + '?' : index);
			}
		}
	},

	0x29: {
		display: {
			label: 'Длительность впрыска (мкс)',
		}
	},

	0x2F: {
		display: {
			label: 'Текущая скорость (км/ч)',
			formula: function(value) {
				return 2 * value;
			}
		}
	},

	0x79: {
		display: {
			label: 'Время открытия форсунки (мс)'
		}
	},

	0x82: {
		display: {
			label: 'БК запас топлива',
		}
	},

	0x83: {
		display: {
			label: 'БК средняя скорость'
		}
	},

	0x84: {
		display: {
			label: 'БК уличная температура'
		}
	},

	0x85: {
		display: {
			label: 'БК расход топлива'
		}
	}


};

var PID_KEYS = Object.keys(PIDS);


function printAtLine(line, key, value) {
	process.stdout.write('\033[' + (line + 1) + ';4H\033[K' + [key, value].join(' = '));
}


function renderPids() {
	var lineNumber = 1;
	for (var pid in PIDS) {
		if (!PIDS.hasOwnProperty(pid)) continue;
		var pidInfo = PIDS[pid];
		[].concat(pidInfo.display).forEach(function(display) {
			var value = pidInfo.value;
			if (display.hasOwnProperty('formula'))
				value = display.formula(value);
			printAtLine(lineNumber++, display.label, value);
		});
	}
}

setInterval(function() {
	rpc.writeNull(parseInt(PID_KEYS[0], 10));
	PID_KEYS.push(PID_KEYS.shift());
}, PID_REQUEST_INTERVAL);

rpc.on('pid', function(key, value) {
	if (!PIDS.hasOwnProperty(key)) return;
	PIDS[key].value = value;
	renderPids();
});


setInterval(function() {
	rpc.emit('pid', PID_KEYS[0], Math.round(Math.random() * 1000));
}, PID_REQUEST_INTERVAL)



