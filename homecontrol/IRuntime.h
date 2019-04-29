#pragma once

#include <optional>
#include "Message.h"
#include "Thread.h"

struct IRuntime {
	virtual void Callback() = 0;
	virtual void HandleMessage(const Message& msg) = 0;
};
