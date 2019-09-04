#include "JSONManager.h"
#include "IRuntimeRegister.h"
#include "IRuntimeConfigure.h"
#include "RuntimeId.h"
#include "StringTools.h"
#include "Sensor.h"
#include "MessageWithDirectAnswer.h"
#include <cpprest/json.h>
#include <chrono>
#include <future>

namespace JSONHandlerN {
	void setOptionalString(web::json::value& json, const utility::string_t& value, const std::optional<std::wstring>& data) {
		if (data) {
			json[value] = web::json::value::string(utility::conversions::to_string_t(*data));
		}
	}

	void setUInt64(web::json::value& json, const utility::string_t& value, unsigned long long data) {
		json[value] = web::json::value::number(data);
	}

	void setInteger(web::json::value& json, const utility::string_t& value, int data) {
		json[value] = web::json::value::number(data);
	}

	void setFloat(web::json::value& json, const utility::string_t& value, float data) {
		json[value] = web::json::value::number(data);
	}

	void setDouble(web::json::value& json, const utility::string_t& value, double data) {
		json[value] = web::json::value::number(data);
	}

	void setString(web::json::value& json, const utility::string_t& value, const std::wstring& data) {
		json[value] = web::json::value::string(data);
	}
}


JSONManager::JSONManager(struct IRuntimeRegister& iRuntimeRegister) :
	m_RuntimeMessageHandler(iRuntimeRegister.RegisterRuntime({ StringTools::AsWstring(__FILE__), runtimeId::JSONManager, std::chrono::milliseconds(100) }, *this))
{
}

JSONManager::~JSONManager() {
}

void JSONManager::Callback()
{
}

void JSONManager::HandleMessage(const Message& msg)
{
}


web::json::value JSONManager::getSensor(int id) const
{
	auto sensor = MassageWithDirectAnswer::SendAndRead<Sensor>(m_RuntimeMessageHandler, Id::Sensor, Sensor(id));

	web::json::value sensorJson;
	JSONHandlerN::setInteger(sensorJson[U("sensor")], U("id"), sensor.m_Id);

	double d = sensor.m_Temperature;
	std::wostringstream stream;
	stream.precision(4);
	stream << d;

	JSONHandlerN::setString(sensorJson[U("sensor")], U("temperature"), stream.str());
	JSONHandlerN::setInteger(sensorJson[U("sensor")], U("humidity"), sensor.m_Humididty);
	JSONHandlerN::setString(sensorJson[U("sensor")], U("name"), sensor.m_Name);

	return sensorJson;

}

