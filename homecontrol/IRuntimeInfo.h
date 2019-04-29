#pragma once

#include <string>
#include "Message.h"
#include "RuntimeId.h"

struct IRuntimeInfo {
	virtual runtimeId GetRuntimeId() const = 0;
	virtual const std::wstring& GetName() const = 0;
};


