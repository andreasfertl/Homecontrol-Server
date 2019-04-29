#pragma once
#include <functional>

//interface for RunQueue
struct IRun {
	virtual void Run(const std::function<void()>&& func) = 0;
	virtual void Run(const std::function<void()>& func) = 0;

};


