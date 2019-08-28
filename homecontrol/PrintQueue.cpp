#include <iostream>
#include <string>
#include "PrintQueue.h"

PrintQueue::PrintQueue()
{
}

PrintQueue::~PrintQueue()
{
}

void PrintQueue::Print(const std::wstring& str)
{
	run([str]() {
		std::wcout << str;
	});
}



