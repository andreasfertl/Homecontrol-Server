#include "JSONManager.h"
#include "IRuntimeRegister.h"
#include "IRuntimeConfigure.h"
#include "RuntimeId.h"
#include "StringTools.h"
#include "Sensor.h"
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
	//setting up lambda & reading of value
	std::promise<web::json::value> sensorValue;
	std::future<web::json::value> sensorValueFuture = sensorValue.get_future();
	
	std::function<void(Message)> answerCallback = [&sensorValue](Message answMessage) {
		if (auto sensor = answMessage.GetValue<Sensor>()) {
			web::json::value sensorJson;
			JSONHandlerN::setInteger(sensorJson[U("sensor")], U("id"), sensor->m_Id);
			JSONHandlerN::setDouble(sensorJson[U("sensor")], U("temperature"), sensor->m_Temperature);
			JSONHandlerN::setInteger(sensorJson[U("sensor")], U("humidity"), sensor->m_Humididty);

			sensorValue.set_value(sensorJson);
		}
		else {
			sensorValue.set_value({});
		}


	};

	//sending message
	m_RuntimeMessageHandler.SendMessage(Message(Cmd::ReadWithDirectAnswer, Id::Sensor, Sensor(id), answerCallback));

	//waiting for answer
	sensorValueFuture.wait();
	return sensorValueFuture.get();
}

