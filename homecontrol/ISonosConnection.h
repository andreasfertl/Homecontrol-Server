#pragma once
#include <string>

struct ISonosConnection {
	virtual void SendPost(const std::wstring& cmd) = 0;
};