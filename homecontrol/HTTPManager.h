#pragma once
#include <memory>
#include "IRuntime.h"
#include "iHTTPHandler.h"
#include "Queue.h"

namespace RequestHandler {

	struct requestHandler {
		int m_RequestId = 1;
		std::map<int, web::http::http_request> m_Map;
		Queue<pplx::task<void>> m_RequestTaskQueue;
		std::mutex m_Mutex;
	};

}

class HTTPManager : public IRuntime, public IHTTPHandler
{


public:
	HTTPManager(struct IPrint& iPrint, struct IRuntimeRegister& iRuntimeRegister, struct ISubscribe& iSubscribe, struct iRESTApi& iRestApi);
	~HTTPManager();

	//IRuntime
	void Callback() override;
	void HandleMessage(const Message& msg) override;

	//IHTTPHandler
	void get(web::http::http_request request) override;
	void put(web::http::http_request message) override;
	void post(web::http::http_request message) override;

private:
	void HandleIncomingRequest(const web::http::http_request& request);

	struct IPrint& m_IPrint;
	struct iRESTApi& m_IRestApi;
	RequestHandler::requestHandler m_RequestHandler;
    struct IRuntimeMessageHandling& m_RuntimeMessageHandler;
};


