var http = require('./');

var server = http.createServer(httpHandler);
server.listen(4000, null, 32000);

var hostname = require('os').hostname();

function httpHandler(req, res){
	res.setHeader('Content-Type', 'application/json');
	res.setHeader('Access-Control-Allow-Origin', '*');
	res.setHeader('Access-Control-Allow-Headers', 'X-Auth, Content-Type, If-None-Match, If-Modified-Since, Origin, X-Date');
	res.setHeader('X-S', hostname);
	
	//console.log(req.method);
	//console.log(req.headers);

	if(req.method === 'OPTIONS') {
		res.end();
		return;
	}
	
	res.end('hello');
}