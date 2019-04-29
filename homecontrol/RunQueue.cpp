#include "RunQueue.h"

RunQueue::RunQueue() :
	m_Queue(),
	m_Run(true),
	m_Event(),
	m_RunThread(&RunQueue::Handler, this)
{
}

RunQueue::~RunQueue()
{
	Run([this]() { this->m_Run = false; });
	m_RunThread.join();
}

void RunQueue::Run(const std::function<void()>&& func)
{
	{
		std::lock_guard<std::mutex> lk(m_Event.m_Mutex);
		m_Queue.push(func);
	}
	m_Event.m_Condition.notify_one();
}

void RunQueue::Run(const std::function<void()>& func)
{
	{
		std::lock_guard<std::mutex> lk(m_Event.m_Mutex);
		m_Queue.push(func);
	}
	m_Event.m_Condition.notify_one();
}


void RunQueue::Handler()
{
	while (m_Run)
	{
		std::unique_lock<std::mutex> lk(m_Event.m_Mutex);
		m_Event.m_Condition.wait(lk, [this]() { return !this->m_Queue.empty(); });
		//clear the queue
		while (!m_Queue.empty())
		{
			m_Queue.front()();
			m_Queue.pop();
		}
	}
}



