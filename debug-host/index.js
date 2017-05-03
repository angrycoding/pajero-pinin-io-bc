var RPC = require('./RPC');
var blessed = require('blessed');
var rpc = new RPC('/dev/tty.usbserial-A921PBNR', 500000);

var screen = blessed.screen({smartCSR: true});
var pureValues = {};

var dataArr = [
	['  ', '_0', '_1', '_2', '_3', '_4', '_5', '_6', '_7', '_8', '_9', '_A', '_B', '_C', '_D', '_E', '_F'],
	['0_', '', '', '', '', '', '', '', '', '', '', '', '', '', '', '', ''],
	['1_', '', '', '', '', '', '', '', '', '', '', '', '', '', '', '', ''],
	['2_', '', '', '', '', '', '', '', '', '', '', '', '', '', '', '', ''],
	['3_', '', '', '', '', '', '', '', '', '', '', '', '', '', '', '', ''],
	['4_', '', '', '', '', '', '', '', '', '', '', '', '', '', '', '', ''],
	['5_', '', '', '', '', '', '', '', '', '', '', '', '', '', '', '', ''],
	['6_', '', '', '', '', '', '', '', '', '', '', '', '', '', '', '', ''],
	['7_', '', '', '', '', '', '', '', '', '', '', '', '', '', '', '', ''],
	['8_', '', '', '', '', '', '', '', '', '', '', '', '', '', '', '', ''],
	['9_', '', '', '', '', '', '', '', '', '', '', '', '', '', '', '', ''],
	['A_', '', '', '', '', '', '', '', '', '', '', '', '', '', '', '', ''],
	['B_', '', '', '', '', '', '', '', '', '', '', '', '', '', '', '', ''],
	['C_', '', '', '', '', '', '', '', '', '', '', '', '', '', '', '', ''],
	['D_', '', '', '', '', '', '', '', '', '', '', '', '', '', '', '', ''],
	['E_', '', '', '', '', '', '', '', '', '', '', '', '', '', '', '', ''],
	['F_', '', '', '', '', '', '', '', '', '', '', '', '', '', '', '', ''],
];

var box = blessed.table({
	top: 0,
	left: 0,
	width: 1,
	height: 1,
	_content: 'Hello {bold}world{/bold}!',
	tags: true,
	border: {
		type: 'line'
	},
	style: {
		border: {
			fg: 'gray'
		}
	},
	data: dataArr
});

screen.append(box);

screen.render();

screen.key(['escape', 'q', 'C-c'], function(ch, key) {
	return process.exit(0);
});

function updatePid(pid, value) {

	var valueStr = ('0' + value.toString(16)).slice(-2);

	if (pureValues[pid] !== value) {
		if (pureValues.hasOwnProperty(pid))
			valueStr = '{green-fg}' + valueStr + '{/green-fg}';
		pureValues[pid] = value;
	}

	dataArr[Math.floor(pid / 16) + 1][pid % 16 + 1] = valueStr;


	box.setData(dataArr);
	screen.render();
}

var pid = 0;

function getRandomInt(min, max) {
	return Math.floor(Math.random() * (max - min + 1)) + min;
}

setInterval(function() {

	updatePid(pid, getRandomInt(0, 3))
	pid++;
	if (pid === 256) {
		pid = 0;
	}

}, 60);

rpc.on('pid', function(key, value) {
	updatePid(key, value);
});
