#include <node.h>
#include <nan.h>

using namespace v8;


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "haywire.h"
#include "src/haywire/khash.h"

#define CRLF "\r\n"

Nan::Persistent<v8::Function> onRequestJsFnc;

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
	Nan::HandleScope scope;
	Response *resp = (Response*)malloc(sizeof(Response));
	resp->request = request;
	resp->response = response;
	resp->user_data = user_data;
	Local<External> external = v8::External::New(v8::Isolate::GetCurrent(), resp);
	
	Local<Object> jsReq = Nan::New<Object>();
	Nan::Set(jsReq, Nan::New<String>("url").ToLocalChecked(), Nan::New<String>(request->url).ToLocalChecked());
	switch(request->method) {
		case HW_HTTP_GET:
			Nan::Set(jsReq, Nan::New<String>("method").ToLocalChecked(), Nan::New<String>("GET").ToLocalChecked()); break;
		case HW_HTTP_POST:
			Nan::Set(jsReq, Nan::New<String>("method").ToLocalChecked(), Nan::New<String>("POST").ToLocalChecked()); break;
		case HW_HTTP_OPTIONS:
			Nan::Set(jsReq, Nan::New<String>("method").ToLocalChecked(), Nan::New<String>("OPTIONS").ToLocalChecked()); break;
		default:
			Nan::Set(jsReq, Nan::New<String>("method").ToLocalChecked(), Nan::New<String>("UNIMPLEMENTED").ToLocalChecked()); break;
	}
	
	
	const char* k;
	const char* v;
	Local<Object> jsHeaders = Nan::New<Object>();

	khash_t(string_hashmap) *h = (kh_string_hashmap_t*)request->headers;
	kh_foreach(h, k, v, { 
		// printf("KEY: %s VALUE: %s\n", k, v);
		jsHeaders->Set(Nan::New<String>(k).ToLocalChecked(), Nan::New<String>(v).ToLocalChecked());
	});
	
	jsReq->Set(Nan::New<String>("headers").ToLocalChecked(), jsHeaders);
	if(request->body->length > 0)
		jsReq->Set(Nan::New<String>("body").ToLocalChecked(), Nan::New<String>(request->body->value, request->body->length).ToLocalChecked());
	else
		jsReq->Set(Nan::New<String>("body").ToLocalChecked(), Nan::Null());
	
	Local<Object> jsRes = Nan::New<Object>();
	jsRes->Set(Nan::New<String>("external").ToLocalChecked(), external);
	
	Local<Value> argv[2] = {
		jsReq,
		jsRes
	};
	
	// hw_set_http_version(resp->response, 1, 1);
	
	Nan::New(onRequestJsFnc)->Call(Nan::GetCurrentContext()->Global(), 2, argv);
	
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

inline void allocStr(hw_string& target, Nan::Utf8String& str) {
	target.length = str.length();
	target.value = (char*)malloc(target.length);
	memcpy(target.value, *str, target.length);
}

NAN_METHOD(JsEnd) {
	Nan::HandleScope scope;
	
	hw_string body;
	
	Local<External> ext = info[0].As<External>();
	void* ptr = ext->Value();
	Response *resp =  static_cast<Response *>(ptr);
	
	hw_string status_code;
	Nan::Utf8String statusCode(info[1]);
	allocStr(status_code, statusCode);
	hw_set_response_status_code(resp->response, &status_code);
	// SETSTRING(status_code, HTTP_STATUS_200);
	// hw_set_response_status_code(resp->response, &status_code);
	
	const Local<Array> props = v8::Handle<v8::Array>::Cast(info[2]);
	const uint32_t length = props->Length();
	
	hw_string *headers = (hw_string*)malloc(sizeof(hw_string)*length);
	
	for (uint32_t i=0 ; i<length ; i+=2)
	{
		//Nan::HandleScope scope;
		const Local<Value> k = props->Get(i);
		Nan::Utf8String headerName(k);
		allocStr(headers[i], headerName);
		
		const Local<Value> v = props->Get(i+1);
		Nan::Utf8String headerValue(v);
		allocStr(headers[i+1], headerValue);
		
		hw_set_response_header(resp->response, &headers[i], &headers[i+1]);
	}
	
	Nan::Utf8String bodyStr(info[3]);
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
	
	info.GetReturnValue().Set(Nan::Undefined());
}

void get_plaintext(http_request* request, hw_http_response* response, void* user_data)
{
    hw_string status_code;
    hw_string content_type_name;
    hw_string content_type_value;
    hw_string body;
    hw_string keep_alive_name;
    hw_string keep_alive_value;
	
	printf("body: %d bytes `%s`\n", request->body->length, request->body->value);
    
    SETSTRING(status_code, HTTP_STATUS_200);
    hw_set_response_status_code(response, &status_code);
    
    SETSTRING(content_type_name, "Content-Type");
    
    SETSTRING(content_type_value, "text/html");
    hw_set_response_header(response, &content_type_name, &content_type_value);
    
    SETSTRING(body, "123");
    hw_set_body(response, &body);
    
    if (request->keep_alive)
    {
        SETSTRING(keep_alive_name, "Connection");
        
        SETSTRING(keep_alive_value, "Keep-Alive");
		hw_set_response_keep_alive(response, true);
        hw_set_response_header(response, &keep_alive_name, &keep_alive_value);
    }
    else
    {
        hw_set_http_version(response, 1, 0);
    }
    
    hw_http_response_send(response, NULL, response_complete);
}

NAN_METHOD(JsListen) {
	printf("JsListen\n");
	Nan::HandleScope scope;
	
	Nan::Utf8String jsPort(info[0]);
	Local<Function> jsFn = Local<Function>::Cast(info[1]);
	onRequestJsFnc.Reset(jsFn);
	
	char route[] = "404";
	configuration config;
	config.parser = "http_parser";
	config.http_listen_address = "0.0.0.0";
	// if((*jsPort)[0] == '/') {
	// 	char *buf = (char*)malloc(jsPort.length());
	// 	memcpy(buf, *jsPort, jsPort.length());
	// 	config.http_listen_port = 0;
	// 	config.unix_file = buf;
	// } else {
	config.http_listen_port = atoi(*jsPort);
		// config.unix_file = NULL;
	// }
	
	config.thread_count = 0;

	/* hw_init_from_config("hello_world.conf"); */
	hw_init_with_config(&config);
	//hw_http_add_route(route, requestHandler, NULL);
	hw_http_add_route("404", requestHandler, NULL);
	hw_http_open();
	
	// NanReturnValue(Nan::New<String>("world"));
	info.GetReturnValue().Set(Nan::Undefined());
}

void init(Handle<Object> exports) {
	Nan::SetMethod(exports, "listen", JsListen);
	Nan::SetMethod(exports, "end", JsEnd);
}

NODE_MODULE(fast_node_http, init)