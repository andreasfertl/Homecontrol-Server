#pragma once

#include "Queue.h"
#include <functional>

class CallbackQueue : public Queue<std::string> {
public:
	CallbackQueue(std::function<void()> writeCallback) : m_WriteCallback(writeCallback) { 
	}

	CallbackQueue() : m_WriteCallback() {
	}

	void Push(const std::string& newElement)
	{
		Queue::Push(newElement);
		if (m_WriteCallback)
			m_WriteCallback();
	}

	void InstallCallback(std::function<void()> writeCallback)
	{
		m_WriteCallback = writeCallback;
	}


private:
	std::function<void()> m_WriteCallback;
};



