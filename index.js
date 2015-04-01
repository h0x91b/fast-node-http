var addon = require('./build/Release/fast-node-http');

setInterval(function(){
	console.log('1');
}, 1000);

console.log('hello()', addon.hello()); // 'world'