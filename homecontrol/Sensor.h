#pragma once


class Sensor
{
public:
	Sensor(){};
	Sensor(const int id, const double temperature, const int humididty) : m_Id(id), m_Temperature(temperature), m_Humididty(humididty) {};
	~Sensor() {};


public:
	int m_Id;
	double m_Temperature;
	int m_Humididty;
};

