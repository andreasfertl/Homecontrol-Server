#pragma once

#include <chrono>
#include <thread>
#include <atomic>
#include "RunQueue.h"


//implements a callback timer
class Timer {

public:
	Timer(std::chrono::milliseconds time, const std::function<void()>& callback);
	~Timer();

	void Stop();

private:
	void Handler();

	std::atomic<bool> m_StopThread;
	std::thread       m_Thread;
};



