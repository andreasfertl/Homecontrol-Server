#pragma once
#include <functional>
#include <string>

class TCPHandler {

public:
	TCPHandler(unsigned int portNr, const struct iNetworkReceive& receiveCallback, std::function<void(std::wstring)> logger);
	~TCPHandler();

	TCPHandler(const TCPHandler&) = delete;
	TCPHandler& operator = (const TCPHandler&) = delete;
	TCPHandler(TCPHandler&&) = default;
	TCPHandler& operator = (TCPHandler&&) = default;

	void sendToAllConnections(const std::string& message);

private:
	std::unique_ptr<class TCPHandlerImpl> m_TCPHandlerImpl;
};

