#pragma once
#include <string>
#include <functional>

class networkMessage {
public:
	networkMessage(const std::string& message, const std::function<void(const std::string&)>& answer) :
		m_Message(message),
		m_Answer(answer)
	{
	}

	const std::string m_Message;
	const std::function<void(const std::string&)> m_Answer;

};

struct iNetworkReceive {
	virtual void receive(const networkMessage& nwMessage) const = 0;
};
