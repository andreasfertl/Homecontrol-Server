#pragma once

#include <string>
#include <chrono>
#include "RuntimeId.h"

struct IRuntimeConfigure {
	std::wstring name;
	runtimeId RuntimeId;
	std::chrono::milliseconds runTime;
};
