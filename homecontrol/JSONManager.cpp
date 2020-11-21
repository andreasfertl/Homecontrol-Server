#include "JSONManager.h"
#include "IRuntimeRegister.h"
#include "IRuntimeConfigure.h"
#include "RuntimeId.h"
#include "StringTools.h"
#include "Sensor.h"
#include "MessageWithDirectAnswer.h"
#include "MessageLightState.h"
#include "BluetoothDevice.h"
#include "TeslaDataStructs.h"
#include <cpprest/json.h>
#include <chrono>
#include <future>
#include <list>

namespace JSONHandlerN {
	void setOptionalString(web::json::value& json, const utility::string_t& value, const std::optional<std::wstring>& data) {
		if (data) {
			json[value] = web::json::value::string(utility::conversions::to_string_t(StringTools::AsString((*data))));
		}
	}

	void setUInt64(web::json::value& json, const utility::string_t& value, unsigned long long data) {
		json[value] = web::json::value::number(static_cast<uint64_t>(data));
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
		json[value] = web::json::value::string(utility::conversions::to_string_t(StringTools::AsString(data)));
	}

	void setSensor(web::json::value& json, const Sensor& sensor) {
		JSONHandlerN::setInteger(json, U("id"), sensor.m_InternalId);
		JSONHandlerN::setDouble(json, U("temperature"), sensor.m_Temperature);
		JSONHandlerN::setInteger(json, U("humidity"), sensor.m_Humididty);
		JSONHandlerN::setString(json, U("name"), sensor.m_Name);
	}

	void setLight(web::json::value& json, const MessageLightState& light) {
		JSONHandlerN::setInteger(json, U("id"), light.m_Id);
		JSONHandlerN::setBool(json, U("on"), light.m_On);
		JSONHandlerN::setString(json, U("name"), light.m_Name);
	}

	void setVehicleState(web::json::value& json, const TeslaDataStructs::vehicleState& vehicleState) {
		JSONHandlerN::setBool(json, U("locked"), vehicleState.m_Locked);
		JSONHandlerN::setDouble(json, U("odometer"), vehicleState.m_OdometerInKm);
		JSONHandlerN::setBool(json, U("fd_window"), vehicleState.m_fp_window);
		JSONHandlerN::setBool(json, U("fp_window"), vehicleState.m_fp_window);
		JSONHandlerN::setBool(json, U("rd_window"), vehicleState.m_rd_window);
		JSONHandlerN::setBool(json, U("rp_window"), vehicleState.m_rp_window);
		JSONHandlerN::setBool(json, U("df"), vehicleState.m_df);
		JSONHandlerN::setBool(json, U("dr"), vehicleState.m_dr);
		JSONHandlerN::setBool(json, U("pf"), vehicleState.m_pf);
		JSONHandlerN::setBool(json, U("pr"), vehicleState.m_pr);
		JSONHandlerN::setBool(json, U("pr"), vehicleState.m_pr);
		JSONHandlerN::setBool(json, U("rt"), vehicleState.m_rt);
	}

	void setDriveState(web::json::value& json, const TeslaDataStructs::driveState& driveState) {
		JSONHandlerN::setUInt64(json, U("heading"), driveState.m_heading);
		JSONHandlerN::setDouble(json, U("latitude"), driveState.m_latitude);
		JSONHandlerN::setDouble(json, U("longitude"), driveState.m_longitude);
		JSONHandlerN::setDouble(json, U("power"), driveState.m_power);
		JSONHandlerN::setDouble(json, U("speed"), driveState.m_speed);
	}

	void setChargeState(web::json::value& json, const TeslaDataStructs::chargeState& chargeState) {
		JSONHandlerN::setUInt64(json, U("usable_battery_level"), chargeState.m_Usable_battery_level);
		JSONHandlerN::setDouble(json, U("battery_rangeInKM"), chargeState.m_EstimatedRangeInKm);
		JSONHandlerN::setDouble(json, U("charger_actual_current"), chargeState.m_Charger_actual_current);
		JSONHandlerN::setUInt64(json, U("charger_voltage"), chargeState.m_Charger_actual_voltage);
		JSONHandlerN::setString(json, U("charging_state"), chargeState.m_Charging_state);
		JSONHandlerN::setDouble(json, U("time_to_full_charge"), chargeState.m_Time_to_full_charge);
    }

	void setClimateState(web::json::value& json, const TeslaDataStructs::climateState& climateState) {
		JSONHandlerN::setDouble(json, U("insideTemp"), climateState.m_Inside_temp);
		JSONHandlerN::setDouble(json, U("outisde_temp"), climateState.m_Outisde_temp);
		JSONHandlerN::setDouble(json, U("driver_temp_setting"), climateState.m_Driver_temp_setting);
		JSONHandlerN::setDouble(json, U("passenger_temp_setting"), climateState.m_Passenger_temp_setting);
		JSONHandlerN::setBool(json, U("seat_heater_left"), climateState.m_Seat_heater_left);
		JSONHandlerN::setBool(json, U("seat_heater_right"), climateState.m_Seat_heater_right);
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
	JSONHandlerN::setSensor(json[U("sensor")], sensor);
	return json;
}

web::json::value JSONManager::getAllSensors() const {
	auto sensors = MassageWithDirectAnswer::SendAndRead<std::list<Sensor>>(m_RuntimeMessageHandler, Id::Sensors, std::list<Sensor>());

	std::vector<web::json::value> sensorsInJson;

	for (const auto& sensor : sensors) {
		web::json::value json;
		JSONHandlerN::setSensor(json, sensor);
		sensorsInJson.push_back(json);
	}

	web::json::value json_array;
	json_array[U("sensor")] = web::json::value::array(sensorsInJson);

	return json_array;
}


web::json::value JSONManager::getLightState(unsigned int internalId) const {
	auto lightstate = MassageWithDirectAnswer::SendAndRead<MessageLightState>(m_RuntimeMessageHandler, Id::LightState, MessageLightState(internalId, false, L""));

	web::json::value json;
	JSONHandlerN::setLight(json[U("light")], lightstate);

	return json;
}

web::json::value JSONManager::setLightState(unsigned int internalId, bool state) const
{
	m_RuntimeMessageHandler.SendMessage(Message(Cmd::Write, Id::LightState, MessageLightState(internalId, state, L"")));
	return getLightState(internalId);
}

web::json::value JSONManager::setBTDevPresent(unsigned int internalId, bool state) const
{
	m_RuntimeMessageHandler.SendMessage(Message(Cmd::Write, Id::BlootoothDevice, BluetoothDevice(internalId, L"", state)));
	return getPresence();
}


web::json::value JSONManager::getAllLights() const
{
	auto lights = MassageWithDirectAnswer::SendAndRead<std::list<MessageLightState>>(m_RuntimeMessageHandler, Id::Lights, std::list<MessageLightState>());

	web::json::value json;

	for (const auto& light : lights) {
		JSONHandlerN::setLight(json, light);
	}

	std::vector<web::json::value> lightsInJson;

	for (const auto& light : lights) {
		web::json::value json;
		JSONHandlerN::setLight(json, light);
		lightsInJson.push_back(json);
	}

	web::json::value json_array;
	json_array[U("light")] = web::json::value::array(lightsInJson);

	return json_array;

	return json;
}

web::json::value JSONManager::getPresence() const
{
	auto presence = MassageWithDirectAnswer::SendAndRead<bool>(m_RuntimeMessageHandler, Id::Presence, true);

	web::json::value json;
	JSONHandlerN::setBool(json, U("presence"), presence);

	return json;
}


web::json::value JSONManager::getVehicleState() const
{
	auto vehicleInformation = MassageWithDirectAnswer::SendAndRead<TeslaDataStructs::vehicleInformation>(m_RuntimeMessageHandler, Id::VehicleInformation, TeslaDataStructs::vehicleInformation());

	web::json::value json;
	JSONHandlerN::setVehicleState(json[U("respsonse")][U("vehicleState")], vehicleInformation.m_VehicleState);
	JSONHandlerN::setDriveState(json[U("respsonse")][U("driveState")], vehicleInformation.m_DriveState);
	JSONHandlerN::setChargeState(json[U("respsonse")][U("chargeState")], vehicleInformation.m_ChargeState);
	JSONHandlerN::setClimateState(json[U("respsonse")][U("climateState")], vehicleInformation.m_ClimateState);

	return json;
}



