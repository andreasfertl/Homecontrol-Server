#pragma once

#include "RunQueue.h"
#include "IPrint.h"


class PrintQueue : private RunQueue, public IPrint {

public:
	PrintQueue();
	~PrintQueue();

	//implementation of IPrint
	void Print(const std::wstring& str) override;
};



