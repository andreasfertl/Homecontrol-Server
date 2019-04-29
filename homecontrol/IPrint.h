#pragma once
#include <string>

//interface for IPrint
struct IPrint {
	virtual void Print(const std::wstring& str) = 0;
};


