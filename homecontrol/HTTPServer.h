#pragma once
#include <memory>
#include <string>

class HTTPServer {

public:
	HTTPServer(struct IPrint& iLogger, const std::wstring& url, struct IHTTPHandler& httpHandler);
	~HTTPServer();

	HTTPServer(const HTTPServer&) = delete;
	HTTPServer& operator = (const HTTPServer&) = delete;
	HTTPServer(HTTPServer&&) = default;
	HTTPServer& operator = (HTTPServer&&) = default;

private:
	std::unique_ptr<class HTTPServerImpl> m_HTTPServerImpl;
};

