#pragma once
#include <string>

class Sensor
{
public:
	Sensor() : m_Id(), m_Temperature(), m_Humididty() {};
	Sensor(const int id) : m_Id(id), m_Temperature(), m_Humididty() {};
	Sensor(const int id, const double temperature, const int humididty, std::wstring name) : m_Id(id), m_Temperature(temperature), m_Humididty(humididty), m_Name(name){};
	~Sensor() {};


public:
	int m_Id;
	double m_Temperature;
	int m_Humididty;
	std::wstring m_Name;
};

