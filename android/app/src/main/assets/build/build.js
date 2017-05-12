var FS = require('fs'),
	Path = require('path'),
	WORK_DIR = process.argv.pop(),
	spawnFile = require('child_process').spawn,
	HTMLCompressorJar = Path.resolve(__dirname, 'java/htmlcompressor-1.5.3.jar'),
	YUICompressorJar = Path.resolve(__dirname, 'java/yuicompressor-2.4.8.jar');

function failWithMessage(message) {
	console.info(message);
	process.exit(1);
}

function spawn(command, args, ret, cwd) {
	var proc = (cwd ? spawnFile(command, args, {cwd: cwd}) : spawnFile(command, args));
	var stdout = '', stderr = '';
	proc.stdout.on('data', function (data) { stdout += data; });
	proc.stderr.on('data', function (data) { stderr += data; });
	proc.on('close', function(code) { ret(stdout, stderr); });
}

function forEachAsync(collection, iterator, ret, index) {

	if (typeof ret !== 'function') ret = function(){};

	if (!(collection instanceof Object)) return ret();
	if (!index) index = 0;

	var keys, key, length;
	var i = -1 + index, calls = 0, looping = false;

	if (collection instanceof Array) {
		length = collection.length;
	} else {
		keys = Object.keys(collection);
		length = keys.length;
	}

	var resume = function() {
		calls += 1;
		if (looping) return;
		looping = true;
		while (calls > 0) {
			calls -= 1, i += 1;
			if (i >= length) return ret();
			key = (keys ? keys[i] : i);
			iterator(collection[key], function(stop) {
				if (stop === true) ret();
				else resume();
			}, key);
		}
		looping = false;
	};
	resume();
}

function findFilesByMask(path, mask, retn, retf, skipPaths) {

	var skipPaths = [].concat(skipPaths);

	function findFilesByMask(path, mask, retn, retf) {

		var maskLen = -(mask.length + 1),
			maskStr = '.' + mask;

		FS.readdir(path, function(err, files) {
			var directories = [];
			forEachAsync(files, function(filePath, next) {
				if (filePath[0] === '.') return next();
				filePath = Path.resolve(path, filePath);
				if (skipPaths.indexOf(filePath) >= 0) next();
				else if (FS.lstatSync(filePath).isDirectory())
					directories.push(filePath), next();
				else if (filePath.slice(maskLen) === maskStr)
					retn(filePath, next);
				else next();
			}, function() {
				if (!directories.length) return retf();
				forEachAsync(directories, function(directory, next) {
					findFilesByMask(directory, mask, retn, next);
				}, retf);
			});
		});
	}

	forEachAsync(mask instanceof Array ? mask : [mask], function(mask, next) {
		findFilesByMask(path, mask, retn, next);
	}, retf);
}

function compressHTML(callback) {
	spawn('java', [
		'-classpath', YUICompressorJar,
		'-jar', HTMLCompressorJar,
		'--type', 'html',
		'--compress-css',
		'--remove-intertag-spaces',
		'--compress-js',
		'--recursive',
		'--remove-surrounding-spaces', 'script,link,style,html,head,title,meta,body,!DOCTYPE,option',
		'--output', WORK_DIR,
		WORK_DIR
	], function(stdout, stderr) {
		if (stderr) failWithMessage(stderr);
		callback();
	});
}

function compressCSS(callback) {

	var cssFiles = [];

	findFilesByMask(WORK_DIR, ['css'], function(file, next) {
		cssFiles.push(file);
		next();
	}, function() {

		spawn('java', [
			'-jar', YUICompressorJar,
			'-o', '.css$:.css'
		].concat(cssFiles), function(stdout, stderr) {
			if (stderr) failWithMessage(stderr);
			callback();
		});

	});

}

compressCSS(function() {
	compressHTML(function() {
		console.info('done');
	});
});