var http = require(__dirname+'/build/Release/fast-node-http');

function Response(context) {
	this.context = context;
	this.headers = [];
	this.statusCode = this.CODES[200];
}

Response.prototype = {
	CODES: {
		200: '200 OK',
		404: '404 Not found',
		403: '403 Forbidden',
		501: 'Not Implemented'
	},
	end: function resEnd(str) {
		if(typeof str === 'undefined') {
			str = ' ';
		} else if(typeof str === 'Buffer') {
			str = str.toString('utf-8');
		}
		http.end(this.context, this.statusCode, this.headers, str);
	},
	writeHead: function writeHead(code) {
		if(typeof this.CODES[code] === 'undefined') {
			code = 501;
		}
		this.statusCode = this.CODES[code];
	},
	setHeader: function setHeader(k,v) {
		this.headers.push(k, v);
	},
	write: function write(data) {
		throw new Error('Not implemented yet');
	}
};

function Server(handler) {
	var reqs = 0;
	var lastReqs = 0;
	var self = this;
	
	this.listen = listen;
	this.close = close;
	
	function listen(port, var1, backlog){
		console.log('listen', arguments);
		http.listen(port, onRequest);
		function onRequest(req, res){
			reqs++;
			req.connection = req.socket = {};
			req.events = {};
			req.on = function(evt, cb) {
				req.events[evt] = cb;
			};
			handler(req, new Response(res.external));
		}
		return self;
	}
	
	function close() {
		throw new Error('Not implemented yet');
	}
}

function createServer(handler) {
	return new Server(handler);
}

module.exports = {
	createServer: createServer,
	globalAgent: {
		maxSockets: 9999999
	}
};



// setInterval(function(){
// 	if(lastReqs !== reqs)
// 		console.log('reqs', reqs, reqs-lastReqs);
// 	lastReqs = reqs;
// }, 1000);
//
// console.log('listen on 8000'); // 'world'