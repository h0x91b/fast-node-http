var http = require('./build/Release/fast-node-http');

var reqs = 0;
var lastReqs = 0;

http.listen(function(req, res){
	reqs++;
	//console.log(req,res);
	http.end(res.external, '200 OK', [
		'Content-Type', 'text/html',
		'Connection', 'Keep-Alive'
	], reqs.toString());
});

setInterval(function(){
	if(lastReqs !== reqs)
		console.log('reqs', reqs, reqs-lastReqs);
	lastReqs = reqs;
}, 1000);

console.log('listen on 8000'); // 'world'