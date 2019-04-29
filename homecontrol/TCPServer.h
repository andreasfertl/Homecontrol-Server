#pragma once
#include <string>
#include <functional>
#include <thread>
#include <atomic>

class TCPServer {

public:
	TCPServer(unsigned int portNr, const std::function<void(const std::string&)>& receiveCallback, class CallbackQueue& txQueue);
	~TCPServer();

private:
	std::atomic<bool> m_Stop;
	std::thread m_SessionHandler;
};

