#include "HTTPServer.h"
#include "iNetworkReceive.h"
#include <cpprest/http_listener.h>
#include <thread>
#include "iRESTApi.h"
#include "Logging.h"
#include "StringTools.h"


class HTTPServerImpl {

public:
	HTTPServerImpl(struct IPrint& iLogger, const std::wstring& url, const iRESTApi& receiveCallback) :
		m_ILogger(iLogger),
		m_ReceiveCallback(receiveCallback),
		m_listener(utility::conversions::to_string_t(StringTools::AsString(url)))
	{
		m_listener.support(web::http::methods::GET, std::bind(&HTTPServerImpl::get, this, std::placeholders::_1));
		m_listener.support(web::http::methods::PUT, std::bind(&HTTPServerImpl::put, this, std::placeholders::_1));
		m_listener.support(web::http::methods::POST, std::bind(&HTTPServerImpl::post, this, std::placeholders::_1));

		try
		{
			m_listener.open().then([this,url]() { Logg(m_ILogger, L"starting to listen on http on: " << url); }).wait();
		}
		catch (std::exception const& e)
		{
			Logg(m_ILogger, e.what());
		}
		catch (...)
		{
			Logg(m_ILogger, L"error on construction of httpserver");
		}
	}

	~HTTPServerImpl() {
		m_listener.close().wait();
	}


private:
	void get(web::http::http_request request) {

		if (auto response = m_ReceiveCallback.parseGetReqeust(web::uri::split_query(request.request_uri().query()))) {
			request.reply(web::http::status_codes::OK, *response).wait();
		}
		else {
			request.reply(web::http::status_codes::BadRequest, U("error in request")).wait();
		}
	}

	void put(web::http::http_request message) {
	}

	void post(web::http::http_request message) {
		return get(message);
	}

	IPrint& m_ILogger;
	const iRESTApi& m_ReceiveCallback;
	web::http::experimental::listener::http_listener m_listener;
};



HTTPServer::HTTPServer(struct IPrint& iLogger, const std::wstring& url, struct iRESTApi& receiveCallback) :
	m_HTTPServerImpl(std::make_unique<HTTPServerImpl>(iLogger, url, receiveCallback))
{
}

HTTPServer::~HTTPServer()
{
}