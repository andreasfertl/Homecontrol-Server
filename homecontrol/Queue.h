#pragma once

#include <queue>
#include <optional>
#include <mutex>

//implements a template for a locked wr/rd queue of any datatype
template<typename ElemtType>
class Queue {

	struct ProtectedQueue {
		std::queue<ElemtType> m_Queue;
		std::mutex m_Mutex;
	};

public:
	Queue() : m_ProtectedQueue() { }
	~Queue() {}

	//template<typename ElemtType>
	void Push(const ElemtType& newElement)
	{

		std::lock_guard<std::mutex> locker(m_ProtectedQueue.m_Mutex);
		m_ProtectedQueue.m_Queue.push(newElement);
	}

	void Push(const ElemtType&& newElement)
	{
		std::lock_guard<std::mutex> locker(m_ProtectedQueue.m_Mutex);
		m_ProtectedQueue.m_Queue.push(newElement);
	}

	std::optional<ElemtType> Pop()
	{
		std::lock_guard<std::mutex> locker(m_ProtectedQueue.m_Mutex);
		if (!m_ProtectedQueue.m_Queue.empty())
		{
			auto element = m_ProtectedQueue.m_Queue.front();
			m_ProtectedQueue.m_Queue.pop();

			return element;
		}
		else
		{
			return std::nullopt;
		}
	}

	size_t Size()
	{
		std::lock_guard<std::mutex> locker(m_ProtectedQueue.m_Mutex);
		return m_ProtectedQueue.m_Queue.size();
	}

private:
	ProtectedQueue m_ProtectedQueue;
};


