#include "Timer.h"

Timer::Timer(std::chrono::milliseconds time, const std::function<void()>& callback) :
	m_StopThread(false),
	m_Thread([this, callback, time]() {
		while (!m_StopThread)
		{
			callback();
			std::this_thread::sleep_for(time);
		}
	})
{
}

void Timer::Stop()
{
	m_StopThread = true;
	if (m_Thread.joinable())
		m_Thread.join();
}

Timer::~Timer()
{
	Stop();
}




