#include <node.h>
#include <nan.h>

using namespace v8;


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "haywire.h"
#include "src/haywire/khash.h"

#define CRLF "\r\n"

v8::Persistent<v8::Function> onRequestJsFnc;

void response_complete(void* user_data)
{
//	if(user_data) {
//		delete user_data;
//	}
}

struct Response {
	http_request* request;
	hw_http_response* response;
	void* user_data;
};

KHASH_MAP_INIT_STR(string_hashmap, char*)

void requestHandler(http_request* request, hw_http_response* response, void* user_data)
{
	NanScope();
	Response *resp = (Response*)malloc(sizeof(Response));
	resp->request = request;
	resp->response = response;
	resp->user_data = user_data;
	Local<External> external = v8::External::New(v8::Isolate::GetCurrent(), resp);
	
	Local<Object> jsReq = NanNew<Object>();
	jsReq->Set(NanNew<String>("url"), NanNew<String>(request->url));
	switch(request->method) {
		case HW_HTTP_GET:
			jsReq->Set(NanNew<String>("method"), NanNew<String>("GET")); break;
		case HW_HTTP_POST:
			jsReq->Set(NanNew<String>("method"), NanNew<String>("POST")); break;
		case HW_HTTP_OPTIONS:
			jsReq->Set(NanNew<String>("method"), NanNew<String>("OPTIONS")); break;
		default:
			jsReq->Set(NanNew<String>("method"), NanNew<String>("UNIMPLEMENTED")); break;
	}
	
	
	const char* k;
	const char* v;
	Local<Object> jsHeaders = NanNew<Object>();

	khash_t(string_hashmap) *h = (kh_string_hashmap_t*)request->headers;
	kh_foreach(h, k, v, { 
		// printf("KEY: %s VALUE: %s\n", k, v);
		jsHeaders->Set(NanNew<String>(k), NanNew<String>(v));
	});
	
	jsReq->Set(NanNew<String>("headers"), jsHeaders);
	
	Local<Object> jsRes = NanNew<Object>();
	jsRes->Set(NanNew<String>("external"), external);
	
	Local<Value> argv[2] = {
		jsReq,
		jsRes
	};
	
	// hw_set_http_version(resp->response, 1, 1);
	
	NanNew(onRequestJsFnc)->Call(NanGetCurrentContext()->Global(), 2, argv);
	
	// hw_string status_code;
	// hw_string content_type_name;
	// hw_string content_type_value;
	// hw_string body;
	// hw_string keep_alive_name;
	// hw_string keep_alive_value;
	//
	// SETSTRING(status_code, HTTP_STATUS_200);
	// hw_set_response_status_code(response, &status_code);
	//
	// SETSTRING(content_type_name, "Content-Type");
	//
	// SETSTRING(content_type_value, "text/html");
	// hw_set_response_header(response, &content_type_name, &content_type_value);
	//
	// SETSTRING(body, "hello world");
	// hw_set_body(response, &body);
	//
	// if (request->keep_alive)
	// {
	// 	SETSTRING(keep_alive_name, "Connection");
	//
	// 	SETSTRING(keep_alive_value, "Keep-Alive");
	// 	hw_set_response_header(response, &keep_alive_name, &keep_alive_value);
	// }
	// else
	// {
	// 	hw_set_http_version(response, 1, 0);
	// }
	//
	// char *user_dataStr = "user_data";
	//
	// hw_http_response_send(response, user_dataStr, response_complete);
}

inline void allocStr(hw_string& target, NanUtf8String& str) {
	target.length = str.length();
	target.value = (char*)malloc(target.length);
	memcpy(target.value, *str, target.length);
}

NAN_METHOD(JsEnd) {
	NanScope();
	
	hw_string body;
	
	Local<External> ext = args[0].As<External>();
	void* ptr = ext->Value();
	Response *resp =  static_cast<Response *>(ptr);
	
	hw_string status_code;
	NanUtf8String statusCode(args[1]);
	allocStr(status_code, statusCode);
	hw_set_response_status_code(resp->response, &status_code);
	
	const Local<Array> props = v8::Handle<v8::Array>::Cast(args[2]);
	const uint32_t length = props->Length();
	
	hw_string *headers = (hw_string*)malloc(sizeof(hw_string)*length);
	
	for (uint32_t i=0 ; i<length ; i+=2)
	{
		//NanScope();
		const Local<Value> k = props->Get(i);
		NanUtf8String headerName(k);
		allocStr(headers[i], headerName);
		
		const Local<Value> v = props->Get(i+1);
		NanUtf8String headerValue(v);
		allocStr(headers[i+1], headerValue);
		
		hw_set_response_header(resp->response, &headers[i], &headers[i+1]);
	}
	
	NanUtf8String bodyStr(args[3]);
	allocStr(body, bodyStr);
	
	hw_set_body(resp->response, &body);
	
	hw_http_response_send(resp->response, resp, response_complete);
	
	free(resp);
	free(status_code.value);
	free(body.value);
	for(uint32_t i=0;i<length;i++) {
		free(headers[i].value);
	}
	free(headers);
	
	NanReturnUndefined();
}

NAN_METHOD(JsListen) {
	NanScope();
	
	NanUtf8String jsPort(args[0]);
	Local<Function> jsFn = Local<Function>::Cast(args[1]);
	NanAssignPersistent(onRequestJsFnc, jsFn);
	
	char route[] = "404";
	configuration config;
	config.http_listen_address = "0.0.0.0";
	if((*jsPort)[0] == '/') {
		char *buf = (char*)malloc(jsPort.length());
		memcpy(buf, *jsPort, jsPort.length());
		config.http_listen_port = 0;
		config.unix_file = buf;
	} else {
		config.http_listen_port = atoi(*jsPort);
		config.unix_file = NULL;
	}

	/* hw_init_from_config("hello_world.conf"); */
	hw_init_with_config(&config);
	hw_http_add_route(route, requestHandler, NULL);
	hw_http_open(0);
	
	// NanReturnValue(NanNew<String>("world"));
	NanReturnUndefined();
}

void init(Handle<Object> exports) {
	NODE_SET_METHOD(exports, "listen", JsListen);
	NODE_SET_METHOD(exports, "end", JsEnd);
}

NODE_MODULE(fast_node_http, init)