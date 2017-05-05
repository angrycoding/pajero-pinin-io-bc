var RPC = require('./RPC');
var blessed = require('blessed');
var rpc = new RPC('/dev/tty.usbserial-A921PBNR', 115200);


var screen = blessed.screen({smartCSR: true});

var selectedCells = [];

var PID_DATA_ARR = [
	['   ', '_ 0', '_ 1', '_ 2', '_ 3', '_ 4', '_ 5', '_ 6', '_ 7', '_ 8', '_ 9', '_ A', '_ B', '_ C', '_ D', '_ E', '_ F'],
	['0 _', '', '', '', '', '', '', '', '', '', '', '', '', '', '', '', ''],
	['1 _', '', '', '', '', '', '', '', '', '', '', '', '', '', '', '', ''],
	['2 _', '', '', '', '', '', '', '', '', '', '', '', '', '', '', '', ''],
	['3 _', '', '', '', '', '', '', '', '', '', '', '', '', '', '', '', ''],
	['4 _', '', '', '', '', '', '', '', '', '', '', '', '', '', '', '', ''],
	['5 _', '', '', '', '', '', '', '', '', '', '', '', '', '', '', '', ''],
	['6 _', '', '', '', '', '', '', '', '', '', '', '', '', '', '', '', ''],
	['7 _', '', '', '', '', '', '', '', '', '', '', '', '', '', '', '', ''],
	['8 _', '', '', '', '', '', '', '', '', '', '', '', '', '', '', '', ''],
	['9 _', '', '', '', '', '', '', '', '', '', '', '', '', '', '', '', ''],
	['A _', '', '', '', '', '', '', '', '', '', '', '', '', '', '', '', ''],
	['B _', '', '', '', '', '', '', '', '', '', '', '', '', '', '', '', ''],
	['C _', '', '', '', '', '', '', '', '', '', '', '', '', '', '', '', ''],
	['D _', '', '', '', '', '', '', '', '', '', '', '', '', '', '', '', ''],
	['E _', '', '', '', '', '', '', '', '', '', '', '', '', '', '', '', ''],
	['F _', '', '', '', '', '', '', '', '', '', '', '', '', '', '', '', ''],
];

var POI_DATA_ARR = [
	['pid', 'val', ' 7 ', ' 6 ', ' 5 ', ' 4 ', ' 3 ', ' 2 ', ' 1 ', ' 0 '],
	['', '', '', '', '', '', '', '', '', ''],
	['', '', '', '', '', '', '', '', '', ''],
	['', '', '', '', '', '', '', '', '', ''],
	['', '', '', '', '', '', '', '', '', ''],
	['', '', '', '', '', '', '', '', '', ''],
	['', '', '', '', '', '', '', '', '', ''],
	['', '', '', '', '', '', '', '', '', ''],
	['', '', '', '', '', '', '', '', '', ''],
	['', '', '', '', '', '', '', '', '', ''],
	['', '', '', '', '', '', '', '', '', ''],
	['', '', '', '', '', '', '', '', '', ''],
	['', '', '', '', '', '', '', '', '', ''],
	['', '', '', '', '', '', '', '', '', ''],
	['', '', '', '', '', '', '', '', '', ''],
	['', '', '', '', '', '', '', '', '', ''],
	['', '', '', '', '', '', '', '', '', ''],
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
	data: PID_DATA_ARR
});

var box2 = blessed.table({
	top: 0,
	width: '100%-104',
	left: 104,
	tags: true,
	border: {
		type: 'line'
	},
	style: {
		border: {
			fg: 'gray'
		}
	},
	data: POI_DATA_ARR
});


function onMouseMove(event, click) {

	var x = Math.floor((event.x - 1) / 6);
	var y = Math.floor(event.y / 2);
	if (event.y % 2 == 0) y--;


	while (selectedCells.length)
		PID_DATA_ARR[selectedCells.shift()][selectedCells.shift()] = selectedCells.shift();


	if (x > 0 && y > 0 && x < 17) {

		selectedCells.push(0, 0, '');

		var pidValue = ((y - 1) * 16 + (x - 1));
		PID_DATA_ARR[0][0] = ('0' + pidValue.toString(16)).slice(-2).toUpperCase();


		selectedCells.push(y, 0, PID_DATA_ARR[y][0]);
		PID_DATA_ARR[y][0] = '{inverse}' + PID_DATA_ARR[y][0] + '{/inverse}';

		selectedCells.push(0, x, PID_DATA_ARR[0][x]);
		PID_DATA_ARR[0][x] = '{inverse}' + PID_DATA_ARR[0][x] + '{/inverse}';

		if (click) {

			if (event.button === 'right') {
				if (PID_DATA_ARR[y][x] === 'x')
					PID_DATA_ARR[y][x] = '';
				else PID_DATA_ARR[y][x] = 'x';
			}

			else {

				var pidIndex = POI_DATA_ARR.findIndex(row => row[0] === PID_DATA_ARR[0][0]);
				if (pidIndex === -1) {
					POI_DATA_ARR.some(function(row, index) {
						if (row[0] === '') {
							POI_DATA_ARR[index][0] = PID_DATA_ARR[0][0];
							return true;
						}
					});
				}

				else {
					POI_DATA_ARR.push(POI_DATA_ARR[pidIndex].fill(''));
					POI_DATA_ARR.splice(pidIndex, 1);
				}


				box2.setData(POI_DATA_ARR);
			}
		}

	}


	box.setData(PID_DATA_ARR);
	screen.render();

}

box.on('mouseout', function() {
	while (selectedCells.length)
		PID_DATA_ARR[selectedCells.shift()][selectedCells.shift()] = selectedCells.shift();
	box.setData(PID_DATA_ARR);
	screen.render();
})

box.on('click', function(event) {
	onMouseMove(event, true);
});

box.on('mousemove', onMouseMove);

screen.append(box);
screen.append(box2);

screen.render();

screen.key(['escape', 'q', 'C-c'], function(ch, key) {
	rpc.exit(function() {
		process.exit(0);
	});
});

function updatePid(pid, value) {

	var x = pid % 16 + 1,
		y = Math.floor(pid / 16) + 1;

	var valueStr = value === null ? 'N' : ('  ' + value.toString()).slice(-3);

	var pidDataV = PID_DATA_ARR[y][x];

	if (pidDataV === 'x') return;

	if (pidDataV && blessed.stripTags(pidDataV) !== valueStr) {
		valueStr = '{inverse}' + valueStr + '{/inverse}'
	}

	PID_DATA_ARR[y][x] = valueStr;


	var pidAsHex = ('0' + pid.toString(16)).slice(-2).toUpperCase();
	var pidIndex = POI_DATA_ARR.findIndex(row => row[0] === pidAsHex);
	if (pidIndex !== -1) {

		if (pidDataV) pidDataV = parseInt(pidDataV, 10);

		POI_DATA_ARR[pidIndex][1] = valueStr;

		for (var c = 0; c < 8; c++)  {
			var old_b_val = POI_DATA_ARR[pidIndex][9 - c];
			var new_b_val = value >> c & 0x01 ? ' 1 ' : ' 0 ';
			if (old_b_val && blessed.stripTags(old_b_val) !== new_b_val) {
				new_b_val = '{inverse}' + new_b_val + '{/inverse}';
			}
			POI_DATA_ARR[pidIndex][9 - c] = new_b_val;
		}

		box2.setData(POI_DATA_ARR);
	}


	box.setData(PID_DATA_ARR);
	screen.render();
}

// var pid = 0;

// setInterval(function() {
// 	updatePid(pid, Math.round(Math.random() * 1000));
// 	pid++;
// 	if (pid === 256) pid = 0;
// }, 0)

rpc.on('pid', function(key, value) {
	updatePid(key, value);
});
