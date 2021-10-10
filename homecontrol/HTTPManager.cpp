#include "HTTPManager.h"
#include "RuntimeId.h"
#include "IRuntimeConfigure.h"
#include "IRuntimeRegister.h"
#include "ISubscribe.h"
#include "iRESTApi.h"
#include "Sensor.h"
#include "MessageWithDirectAnswer.h"
#include <chrono>


namespace {

	const std::chrono::milliseconds maxTimeoutForRequest(1000);

	template<typename T>
	std::optional<T> readData(IRuntimeMessageHandling& iRuntime, RequestHandler::requestHandler& requestHandler, int requestId, Id id, T data) {
		std::promise<std::optional<T>> value;
		std::future<std::optional<T>> valueFuture = value.get_future();

		std::function<void(Message)> answerCallback = [&value, &requestHandler, &iRuntime, requestId](Message answMessage) {
			if (auto rxData = answMessage.GetValue<T>()) {
				try {
					std::function<void()> sendBackToOwnContext = [&value, &requestHandler, requestId, &rxData]() {
						//std::lock_guard<std::mutex> locker(requestHandler.m_Mutex);
						//mark it handled by removing, a second reply will result in nullopt
						auto it = requestHandler.m_Map.find(requestId);
						if (it != requestHandler.m_Map.end()) {
							requestHandler.m_Map.erase(it);
							value.set_value(*rxData);
						}
					};
					iRuntime.PushMessageToRuntimeHandler(Message(Cmd::Run, Id::Run, sendBackToOwnContext));
				}
				catch (...) {
					;
				}
			}
		};

		//sending message
		iRuntime.SendMessage(Message(Cmd::ReadWithDirectAnswer, id, data, answerCallback));

		//waiting for THE one answer (with timeout), it wouldn't be several since i protect it with the requestHandler
		valueFuture.wait_for(maxTimeoutForRequest);
		return valueFuture.get();
	}

	template<typename T>
	void TriggerReadDataAndAnswerRequest(IRuntimeMessageHandling& iRuntime, RequestHandler::requestHandler& requestHandler, int requestId, Id id, T data) {
		std::function<void(Message)> answerCallback = [&iRuntime, &requestHandler, requestId](Message answMessage) {
			if (auto rxData = answMessage.GetValue<T>()) {
				try {
					std::function<void()> sendBackToOwnContext = [&requestHandler, requestId, rxData]() {
						//mark it handled by removing, a second reply will result in nothing
						auto it = requestHandler.m_Map.find(requestId);
						if (it != requestHandler.m_Map.end()) {
							auto data = *rxData;
							//register task(reply)
							requestHandler.m_RequestTaskQueue.Push(it->second.reply(web::http::status_codes::OK, data.asJson()));
							requestHandler.m_Map.erase(it);
						}
					};
					iRuntime.PushMessageToRuntimeHandler(Message(Cmd::Run, Id::Run, sendBackToOwnContext));
				}
				catch (...) {
				}
			}
		};

		//trigger read data
		iRuntime.SendMessage(Message(Cmd::ReadWithDirectAnswer, id, data, answerCallback));
	}



	int registerNewRequest(RequestHandler::requestHandler& requestHandler, const web::http::http_request& request)
	{
		//std::lock_guard<std::mutex> locker(requestHandler.m_Mutex);
		//register request, to ensure that we only make one response
		auto idOfMyRequest = requestHandler.m_RequestId;
		requestHandler.m_RequestId++; //prepare for next round
		//register my new request
		requestHandler.m_Map[idOfMyRequest] = request;

		return idOfMyRequest;
	}
}


HTTPManager::HTTPManager(IPrint& iPrint, struct IRuntimeRegister& iRuntimeRegister, ISubscribe& iSubscribe, struct iRESTApi& iRestApi) :
	m_IPrint(iPrint),
	m_IRestApi(iRestApi),
	m_RequestHandler(),
	m_RuntimeMessageHandler(iRuntimeRegister.RegisterRuntime({ StringTools::AsWstring(__FILE__), runtimeId::HTTPManager, std::chrono::milliseconds(100) }, *this))
{
}

HTTPManager::~HTTPManager()
{
	m_RuntimeMessageHandler.Stop();
}

void HTTPManager::HandleIncomingRequest(const web::http::http_request& request) {
	auto queries = web::uri::split_query(request.request_uri().query());

	for (auto& query : queries) {
		auto [command, value] = query;

		//function?
		if (command == L"sensor") {
			auto id = stoi(value);
			auto idOfMyRequest = registerNewRequest(m_RequestHandler, request);

			TriggerReadDataAndAnswerRequest(m_RuntimeMessageHandler, m_RequestHandler, idOfMyRequest, Id::Sensor, Sensor(id));
			//register task(reply)
			//m_RequestHandler.m_RequestTaskQueue.Push(request.reply(web::http::status_codes::OK, answer->asJson()));
		}
	}
}




//different context then callback!
void HTTPManager::get(web::http::http_request request)
{
	std::function<void()> callback = [this, request]() {
		HandleIncomingRequest(request);
	};

	m_RuntimeMessageHandler.PushMessageToRuntimeHandler(Message(Cmd::Run, Id::Run, callback));
}

//different context then callback!
void HTTPManager::put(web::http::http_request message)
{
}

//different context then callback!
void HTTPManager::post(web::http::http_request message)
{
}

void HTTPManager::Callback()
{
	//std::lock_guard<std::mutex> locker(m_RequestHandler.m_Mutex);

	while (auto requestToCleanup = m_RequestHandler.m_RequestTaskQueue.Pop()) {
		if (!requestToCleanup->is_done()) //not yet done -> wait for it to finish
			requestToCleanup->wait();
	}
}

void HTTPManager::HandleMessage(const Message& msg)
{
}





