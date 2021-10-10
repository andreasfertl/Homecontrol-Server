#pragma once
#include <cpprest/http_listener.h>

struct IHTTPHandler {
	virtual void get(web::http::http_request request) = 0;
	virtual void put(web::http::http_request message) = 0;
	virtual void post(web::http::http_request message) = 0;
};
