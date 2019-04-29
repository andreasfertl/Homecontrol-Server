#pragma once


struct MessageLightState
{
public:
	MessageLightState() : m_Id(), m_On() {};
	MessageLightState(unsigned int id, bool on) : m_Id(id), m_On(on) {};

	unsigned int m_Id;
	bool m_On;
};