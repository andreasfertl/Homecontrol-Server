#pragma once

#include <thread>
#include <mutex>
#include <queue>
#include <condition_variable>
#include "Runner.h"


//implements a queue which handles functions in sequence
//can be used to run functions in sequence, and to protect concurrent data access by
//putting the according functions to access the data in sequence
class RunQueue : public IRun {

public:
	RunQueue();
	~RunQueue();

	//implementation of IRun
	void Run(const std::function<void()>&& func) override;
	void Run(const std::function<void()>& func) override;


private:
	void Handler();

private:

	struct Event {
		std::mutex m_Mutex;
		std::condition_variable m_Condition;
	};

	std::queue<std::function<void()>> m_Queue;
	bool m_Run;
	Event m_Event;
	std::thread m_RunThread;
};



