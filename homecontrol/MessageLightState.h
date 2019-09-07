#pragma once
#include <string>

struct MessageLightState
{
public:
	MessageLightState() : m_Id(), m_On() {};
	MessageLightState(unsigned int id, bool on, const std::wstring& name) : m_Id(id), m_On(on), m_Name(name) {};

	unsigned int m_Id;
	bool m_On;
	std::wstring m_Name;
};