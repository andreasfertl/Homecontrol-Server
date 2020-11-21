#pragma once

#include <chrono>
#include <thread>
#include <atomic>
#include <functional>

class OneShotTimer {

public:
	OneShotTimer(std::chrono::milliseconds time, const std::function<void()>& callback);
	~OneShotTimer();

private:
	std::thread       m_Thread;
};



#pragma once
