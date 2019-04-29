#pragma once

#include <functional>

struct IUpdateLight {
	virtual void SetLamp(unsigned int lampId, bool state) = 0;
};

template<typename interfaceImplementer>
struct IRunInterface {
public:
	IRunInterface(std::function<void(interfaceImplementer& interface)> toRun) { f = toRun; }
	void operator()(interfaceImplementer& interface) { f(interface); }

private:
	std::function<void(interfaceImplementer& interface)> f;
};