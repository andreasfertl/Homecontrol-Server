#pragma once

#include "seriealizeFunctions.h"
#include "IPrint.h"


class PrintQueue : private seriealizeFunctions, public IPrint {

public:
	PrintQueue();
	~PrintQueue();

	//implementation of IPrint
	void Print(const std::wstring& str) override;
};



