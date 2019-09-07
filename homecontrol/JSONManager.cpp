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
	JSONHandlerN::setInteger(json[U("sensor")], U("id"), sensor.m_Id);
	JSONHandlerN::setString(json[U("sensor")], U("temperature"), StringTools::FormatWithXDigits(sensor.m_Temperature, 4));
	JSONHandlerN::setInteger(json[U("sensor")], U("humidity"), sensor.m_Humididty);
	JSONHandlerN::setString(json[U("sensor")], U("name"), sensor.m_Name);

	return json;
}

web::json::value JSONManager::getLightState(unsigned int internalId) const {
	auto lightstate = MassageWithDirectAnswer::SendAndRead<MessageLightState>(m_RuntimeMessageHandler, Id::LightState, MessageLightState(internalId, false, L""));

	web::json::value json;
	JSONHandlerN::setInteger(json[U("lightstate")], U("id"), lightstate.m_Id);
	JSONHandlerN::setBool(json[U("lightstate")], U("on"), lightstate.m_On);
	JSONHandlerN::setString(json[U("lightstate")], U("name"), lightstate.m_Name);

	return json;

}

web::json::value JSONManager::setLightState(unsigned int internalId, bool state) const
{
	m_RuntimeMessageHandler.SendMessage(Message(Cmd::Write, Id::LightState, MessageLightState(internalId, state, L"")));
	return getLightState(internalId);
}


