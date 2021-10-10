#include "HTTPServer.h"
#include "iNetworkReceive.h"
#include <cpprest/http_listener.h>
#include <thread>
#include "iRESTApi.h"
#include "Logging.h"
#include "StringTools.h"
#include "iHTTPHandler.h"

class HTTPServerImpl {

public:
	HTTPServerImpl(struct IPrint& iLogger, const std::wstring& url, IHTTPHandler& httpHandler) :
		m_ILogger(iLogger),
		m_HttpHandler(httpHandler),
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
		try
		{
			m_listener.close().wait();
		}
		catch (std::exception const& e)
		{
			Logg(m_ILogger, e.what());
		}
		catch (...)
		{
			Logg(m_ILogger, L"error on destruction of the httpserver");
		}
	}


private:
	void get(web::http::http_request request) {
		return m_HttpHandler.get(request);
	}

	void put(web::http::http_request message) {
		return m_HttpHandler.put(message);
	}

	void post(web::http::http_request message) {
		return m_HttpHandler.post(message);
	}

	IPrint& m_ILogger;
	IHTTPHandler& m_HttpHandler;
	web::http::experimental::listener::http_listener m_listener;
};



HTTPServer::HTTPServer(struct IPrint& iLogger, const std::wstring& url, IHTTPHandler& httpHandler) :
	m_HTTPServerImpl(std::make_unique<HTTPServerImpl>(iLogger, url, httpHandler))
{
}

HTTPServer::~HTTPServer()
{
}