#include "JSONManager.h"
#include "IRuntimeRegister.h"
#include "IRuntimeConfigure.h"
#include "RuntimeId.h"
#include "StringTools.h"
#include "Sensor.h"
#include "MessageWithDirectAnswer.h"
#include "MessageLightState.h"
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

	void setBool(web::json::value& json, const utility::string_t& value, bool data) {
		json[value] = web::json::value::boolean(data);
	}

	void setString(web::json::value& json, const utility::string_t& value, const std::wstring& data) {
		json[value] = web::json::value::string(data);
	}

	void setSensor(web::json::value& json, const Sensor& sensor) {
		auto id = utility::conversions::to_string_t(std::to_wstring(sensor.m_Id));
		JSONHandlerN::setInteger(json[U("sensor")][id], U("id"), sensor.m_Id);
		JSONHandlerN::setDouble(json[U("sensor")][id], U("temperature"), sensor.m_Temperature);
		JSONHandlerN::setInteger(json[U("sensor")][id], U("humidity"), sensor.m_Humididty);
		JSONHandlerN::setString(json[U("sensor")][id], U("name"), utility::conversions::to_string_t(sensor.m_Name));
	}

	void setLight(web::json::value& json, const MessageLightState& light) {
		auto id = utility::conversions::to_string_t(std::to_wstring(light.m_Id));
		JSONHandlerN::setInteger(json[U("light")][id], U("id"), light.m_Id);
		JSONHandlerN::setBool(json[U("light")][id], U("on"), light.m_On);
		JSONHandlerN::setString(json[U("light")][id], U("name"), utility::conversions::to_string_t(light.m_Name));
	}
}


JSONManager::JSONManager(struct IRuntimeRegister& iRuntimeRegister) :
	m_RuntimeMessageHandler(iRuntimeRegister.RegisterRuntime({ StringTools::AsWstring(__FILE__), runtimeId::JSONManager, std::chrono::milliseconds(100) }, *this))
{
}

JSONManager::~JSONManager() {
}

void JSONManager::Callback() {
}

void JSONManager::HandleMessage(const Message& msg) {
}


web::json::value JSONManager::getSensor(unsigned int internalId) const {
	auto sensor = MassageWithDirectAnswer::SendAndRead<Sensor>(m_RuntimeMessageHandler, Id::Sensor, Sensor(internalId));

	web::json::value json;
	JSONHandlerN::setSensor(json, sensor);
	return json;
}

web::json::value JSONManager::getAllSensors() const {
	auto sensors = MassageWithDirectAnswer::SendAndRead<std::list<Sensor>>(m_RuntimeMessageHandler, Id::Sensors, std::list<Sensor>());

	web::json::value json;

	for (const auto& sensor : sensors) {
		JSONHandlerN::setSensor(json, sensor);
	}

	return json;
}


web::json::value JSONManager::getLightState(unsigned int internalId) const {
	auto lightstate = MassageWithDirectAnswer::SendAndRead<MessageLightState>(m_RuntimeMessageHandler, Id::LightState, MessageLightState(internalId, false, L""));

	web::json::value json;
	JSONHandlerN::setLight(json, lightstate);

	return json;

}

web::json::value JSONManager::setLightState(unsigned int internalId, bool state) const
{
	m_RuntimeMessageHandler.SendMessage(Message(Cmd::Write, Id::LightState, MessageLightState(internalId, state, L"")));
	return getLightState(internalId);
}

web::json::value JSONManager::getAllLights() const
{
	auto lights = MassageWithDirectAnswer::SendAndRead<std::list<MessageLightState>>(m_RuntimeMessageHandler, Id::Lights, std::list<MessageLightState>());

	web::json::value json;

	for (const auto& light : lights) {
		JSONHandlerN::setLight(json, light);
	}

	return json;
}


