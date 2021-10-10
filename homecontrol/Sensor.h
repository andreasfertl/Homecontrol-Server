#pragma once
#include <string>
#include "JsonHelper.h"

class Sensor
{
public:
	Sensor() : m_InternalId(), m_Id(), m_Temperature(), m_Humididty() {};
	Sensor(unsigned int internalId) : m_InternalId(internalId), m_Id(), m_Temperature(), m_Humididty() {};
	Sensor(unsigned int internalId, int id, double temperature, int humididty, std::wstring name) : m_InternalId(internalId), m_Id(id), m_Temperature(temperature), m_Humididty(humididty), m_Name(name){};
	~Sensor() {};

	web::json::value asJson() {
		web::json::value json;
		JsonHelperN::setInteger(json, U("id"), m_InternalId);
		JsonHelperN::setDouble(json, U("temperature"), m_Temperature);
		JsonHelperN::setInteger(json, U("humidity"), m_Humididty);
		JsonHelperN::setString(json, U("name"), m_Name);
		return json;
	}

public:
	unsigned int m_InternalId;
	int m_Id;
	double m_Temperature;
	int m_Humididty;
	std::wstring m_Name;
};

