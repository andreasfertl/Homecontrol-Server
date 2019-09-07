#pragma once
#include <string>

class Sensor
{
public:
	Sensor() : m_InternalId(), m_Id(), m_Temperature(), m_Humididty() {};
	Sensor(unsigned int internalId) : m_InternalId(internalId), m_Id(), m_Temperature(), m_Humididty() {};
	Sensor(unsigned int internalId, int id, double temperature, int humididty, std::wstring name) : m_InternalId(internalId), m_Id(id), m_Temperature(temperature), m_Humididty(humididty), m_Name(name){};
	~Sensor() {};

public:
	unsigned int m_InternalId;
	int m_Id;
	double m_Temperature;
	int m_Humididty;
	std::wstring m_Name;
};

