#pragma once

class MandolynSensor
{
public:
	MandolynSensor(int id, double temp, int humidity) : m_Id(id), m_Temp(temp), m_Humidity(humidity) {};

private:
	int    m_Id;
	double m_Temp;
	int    m_Humidity;

};
