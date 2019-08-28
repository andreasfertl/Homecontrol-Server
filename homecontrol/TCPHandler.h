#pragma once
#include <functional>

class TCPHandler {

public:
	TCPHandler(unsigned int portNr, const struct iNetworkReceive& receiveCallback);
	~TCPHandler();

	TCPHandler(const TCPHandler&) = delete;
	TCPHandler& operator = (const TCPHandler&) = delete;
	TCPHandler(TCPHandler&&) = default;
	TCPHandler& operator = (TCPHandler&&) = default;

private:
	std::unique_ptr<class TCPHandlerImpl> m_TCPHandlerImpl;
};

