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
	Run([str]() {
		std::wcout << str;
	});
}



