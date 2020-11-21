#include "OneShotTimer.h"

OneShotTimer::OneShotTimer(std::chrono::milliseconds time, const std::function<void()>& callback) :
	m_Thread([callback, time]() {
		std::this_thread::sleep_for(time);
		callback(); //TODO need to improve this one - just a bit to stupid -> desctructor will wait for callback..... (which could be a long time)
	})
{
}

OneShotTimer::~OneShotTimer()
{
	if (m_Thread.joinable())
		m_Thread.join();
}




