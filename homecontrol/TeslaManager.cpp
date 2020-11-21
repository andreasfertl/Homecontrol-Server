#include "TeslaManager.h"
#include "RuntimeId.h"
#include "IRuntimeConfigure.h"
#include "IRuntimeRegister.h"
#include "IGetConfiguration.h"
#include "ISubscribe.h"
#include "IOauth2.h"
#include <cpprest/json.h>
#include "TeslaDataStructs.h"
#include "IGetConfiguration.h"




namespace {
	const unsigned int drivingTimeout(5); //Minutes

	inline web::json::value getResponseAsObject(const std::wstring& response) {
#ifdef _WIN32
		auto responseToParse = response;
#else 
		auto responseToParse = StringTools::AsString(response);
#endif 

		auto jsonClimateState = web::json::value::parse(responseToParse);
		return jsonClimateState[U("response")];
	}

	inline bool validRespone(const std::wstring& response) {
		auto checkResponse = getResponseAsObject(response);

		if (!checkResponse.is_null())
			return true;
		else
			return false;
	}

	TeslaDataStructs::climateState parseClimateState(const std::wstring& climateState) {
		auto response = getResponseAsObject(climateState);

		auto inside_temp = response[U("inside_temp")].as_double();
		auto outside_temp = response[U("outside_temp")].as_double();
		auto driver_temp_setting = response[U("driver_temp_setting")].as_double();
		auto passenger_temp_setting = response[U("passenger_temp_setting")].as_double();
		auto seat_heater_left = response[U("seat_heater_left")].as_integer() != 0;
		auto seat_heater_right = response[U("seat_heater_right")].as_integer() != 0;

		return TeslaDataStructs::climateState(inside_temp, outside_temp, driver_temp_setting, passenger_temp_setting, seat_heater_left, seat_heater_right);
	}

	TeslaDataStructs::chargeState parseChargeState(const std::wstring& chargeState) {
		auto response = getResponseAsObject(chargeState);

		auto usable_battery_level   = response[U("usable_battery_level")].as_integer();
		auto battery_range          = response[U("battery_range")].as_double();
		auto charger_actual_current = response[U("charger_actual_current")].as_double();
		auto charger_actual_voltage = response[U("charger_voltage")].as_integer();
		auto charging_state         = response[U("charging_state")].as_string();
		auto time_to_full_charge    = response[U("time_to_full_charge")].as_double();
#ifdef _WIN32
		auto charging_state_Wide = charging_state;
#else 
		auto charging_state_Wide = StringTools::AsWstring(charging_state);
#endif 
		return TeslaDataStructs::chargeState(usable_battery_level, battery_range, charger_actual_current, charger_actual_voltage, charging_state_Wide, time_to_full_charge);
	}

	TeslaDataStructs::vehicleState parseVehicleState(const std::wstring& vehicleState) {
		auto response = getResponseAsObject(vehicleState);

		auto locked = response[U("locked")].as_bool();
		auto odometer = response[U("odometer")].as_double();
		auto fd_window = response[U("fd_window")].as_integer() == 0;
		auto fp_window = response[U("fp_window")].as_integer() == 0;
		auto rd_window = response[U("rd_window")].as_integer() == 0;
		auto rp_window = response[U("rp_window")].as_integer() == 0;
		auto df        = response[U("df")].as_integer() == 0;
		auto dr        = response[U("dr")].as_integer() == 0;
		auto pf        = response[U("pf")].as_integer() == 0;
		auto pr        = response[U("pr")].as_integer() == 0;
		auto ft        = response[U("ft")].as_integer() == 0;
		auto rt        = response[U("rt")].as_integer() == 0;
	
		return TeslaDataStructs::vehicleState(locked, odometer, fd_window, fp_window, rd_window, rp_window, df, dr, pf, pr, ft, rt);
	}

	TeslaDataStructs::driveState parseDriveState(const std::wstring& driveState) {
		auto response = getResponseAsObject(driveState);

		auto heading = response[U("heading")].as_integer();
		auto latitude = response[U("latitude")].as_double();
		auto longitude = response[U("longitude")].as_double();
		auto power = response[U("power")].as_double();
		double speed = 0.0f;
		if (!response[U("speed")].is_null())
			speed = response[U("speed")].as_double();

		return TeslaDataStructs::driveState(heading, latitude, longitude, power, speed);
	}

	std::optional<TeslaDataStructs::vehicleInformation> ReadDataFromTeslaWebsite(TeslaConnection& teslaConnection)
	{
		TeslaDataStructs::vehicleInformation vehicleInformation;
		
		auto response = teslaConnection.get(L"climate_state");
		if (validRespone(response)) {
			vehicleInformation.m_ClimateState = parseClimateState(response);
		}
		else {
			return std::nullopt;
		}

		response = teslaConnection.get(L"charge_state");
		if (validRespone(response)) {
			vehicleInformation.m_ChargeState = parseChargeState(response);
		}
		else {
			return std::nullopt;
		}

		response = teslaConnection.get(L"vehicle_state");
		if (validRespone(response)) {
			vehicleInformation.m_VehicleState = parseVehicleState(response);
		}
		else {
			return std::nullopt;
		}

		response = teslaConnection.get(L"drive_state");
		if (validRespone(response)) {
			vehicleInformation.m_DriveState = parseDriveState(response);
		}
		else {
			return std::nullopt;
		}

		return vehicleInformation;
	}

	bool isCarDriving(const TeslaDataStructs::driveState& driveState) {
		static bool driving = false;
		static auto lastTimeDrove = std::chrono::system_clock::now();

		if (driveState.m_speed != 0.0f) {
			driving = true;
			lastTimeDrove = std::chrono::system_clock::now();
		}
		else {
			if (driving) //was driving, now the first time 0 -> delay 5minutes 
			{
				auto now = std::chrono::system_clock::now();
				if (std::chrono::duration_cast<std::chrono::minutes>(now - lastTimeDrove).count() > drivingTimeout)
					driving = false;
			}
		}
		return driving;
	}

	bool isCarCharging(const TeslaDataStructs::chargeState& chargeState) {
		if (chargeState.m_Charger_actual_current != 0.0f) {
			return true;
		}
		else {
			return false;
		}
	}


}

TeslaManager::TeslaManager(IPrint& iPrint, struct IConfigurationTeslaManager& iGetConfigurationIRuntimeRegister,struct IRuntimeRegister &iRuntimeRegister, ISubscribe& iSubscribe) :
	m_IPrint(iPrint),
	m_TeslaAuthStorage(iGetConfigurationIRuntimeRegister.GetConfigurationTeslaManager().m_ConfigToken),
    m_RetryWakeUps(iGetConfigurationIRuntimeRegister.GetConfigurationTeslaManager().m_retryWakeUps),
	m_RetrySleepInSeconds(iGetConfigurationIRuntimeRegister.GetConfigurationTeslaManager().m_retrySleepInSeconds),
	m_fastReadTimeInSeconds(iGetConfigurationIRuntimeRegister.GetConfigurationTeslaManager().m_fastReadTimeInSeconds),
	m_verySlowReadTimeInSeconds(iGetConfigurationIRuntimeRegister.GetConfigurationTeslaManager().m_verySlowReadTimeInSeconds),
	m_maxSlowingRateForVerySlowReads(iGetConfigurationIRuntimeRegister.GetConfigurationTeslaManager().m_maxSlowingRateForVerySlowReads),
	m_TeslaDataConnection(iPrint, m_TeslaAuthStorage, iGetConfigurationIRuntimeRegister.GetConfigurationTeslaManager().m_vehicleId, iGetConfigurationIRuntimeRegister.GetConfigurationTeslaManager().m_controlUrl, L"data_request"),
	m_VehicleInformation(),
	m_State(ReaderStates::CheckState),
	m_Rate(1),
	m_OneShotTimer(),
	m_RuntimeMessageHandler(iRuntimeRegister.RegisterRuntime({ StringTools::AsWstring(__FILE__), runtimeId::TeslaManager, std::chrono::seconds(1) }, * this))
{
	iSubscribe.Subscribe({ Id::VehicleInformation, m_RuntimeMessageHandler });
}

TeslaManager::~TeslaManager()
{
	m_RuntimeMessageHandler.Stop();
}

void TeslaManager::Callback()
{
	switch (m_State) {
		case ReaderStates::WaitForStateChange: 
			break;

		case ReaderStates::CheckState:
		{
			Logg(m_IPrint, L"Check state");
			try {
				auto response = ReadDataFromTeslaWebsite(m_TeslaDataConnection);
				if (response) {
					m_VehicleInformation = *response;
					if (isCarDriving(m_VehicleInformation.m_DriveState))
						m_State = ReaderStates::CarIsAwakeAndDriving;
					else if (isCarCharging(m_VehicleInformation.m_ChargeState))
						m_State = ReaderStates::CarIsAwakeAndCharging;
					else
						m_State = ReaderStates::CarIsAwake;
				}
				else {
					m_State = ReaderStates::EnterCarIsSleepingMode;
				}
			}
			catch (...) {
				//got an exception - reading very slow now
				Logg(m_IPrint, L"exception on reading from Tesla Webside - slow reading");
				m_OneShotTimer = std::make_unique<OneShotTimer>(std::chrono::seconds(m_verySlowReadTimeInSeconds), [this]() {
					m_State = ReaderStates::CheckState;
				});
				//go back to idle and wait for change through timer
				m_State = ReaderStates::WaitForStateChange;
			}
			break;
		}

		case ReaderStates::CarIsAwake:
		{
			Logg(m_IPrint, L"Car is awake");
			//treat it the same as sleeping, don't want to stress the car
			m_State = ReaderStates::EnterCarIsSleepingMode;
			break;
		}

		case ReaderStates::CarIsAwakeAndDriving:
		{
			Logg(m_IPrint, L"Car is awake and driving");
			//as long as we are driving we read quite often
			m_OneShotTimer = std::make_unique<OneShotTimer>(std::chrono::seconds(m_fastReadTimeInSeconds), [this]() {
				m_State = ReaderStates::CheckState;
			});
			//go back to idle and wait for change through timer
			m_State = ReaderStates::WaitForStateChange;
			break;
		}

		case ReaderStates::CarIsAwakeAndCharging:
		{
			Logg(m_IPrint, L"Car is awake and charging");
			//as long as we are charging we read quite often
			m_OneShotTimer = std::make_unique<OneShotTimer>(std::chrono::seconds(m_fastReadTimeInSeconds), [this]() {
				m_State = ReaderStates::CheckState;
			});
			//go back to idle and wait for change through timer
			m_State = ReaderStates::WaitForStateChange;
			break;
		}

		case ReaderStates::EnterCarIsSleepingMode: {
			Logg(m_IPrint, L"enter car is sleeping mode");
			m_Rate = 1;
			m_State = ReaderStates::CarIsSleeping;
			break;
		}

		case ReaderStates::CarIsSleeping:
		{
			Logg(m_IPrint, L"car is sleeping mode, rate: " << m_Rate);
			//as long as we are sleeping we don't ask often
			m_OneShotTimer = std::make_unique<OneShotTimer>(std::chrono::seconds(m_verySlowReadTimeInSeconds) *m_Rate.load(), [this]() {
				//check if car is awake
				auto response = ReadDataFromTeslaWebsite(m_TeslaDataConnection);
				if (response) { //awake
					m_State = ReaderStates::CheckState; //-> go check state
				}
				else { //nope
					Logg(m_IPrint, L"car is sleeping, trying to wake, rate: " << m_Rate);
					//ok try to wake him (with retries)
					for (auto i = 0u; i < m_RetryWakeUps; i++) {
						auto response = m_TeslaDataConnection.post(L"wake_up");
						auto responseObject = getResponseAsObject(response);

						auto stateOfCar = responseObject[U("state")].as_string();
						if (StringTools::AsWstring(stateOfCar) == L"online") {
							Logg(m_IPrint, L"woke car");
							//managed/needed to wake him, next time not so often
							m_Rate++;
							if (m_Rate >= m_maxSlowingRateForVerySlowReads)
								m_Rate = m_maxSlowingRateForVerySlowReads;
							break;
						}
						else {
							Logg(m_IPrint, L"didn't wake care, sleeping");
							std::this_thread::sleep_for(std::chrono::seconds(m_RetrySleepInSeconds));
						}
					}
					//ok now we can try to read
					m_State = ReaderStates::CheckState;
				}
			});
			//go back to idle and wait for change through timer
			m_State = ReaderStates::WaitForStateChange;
			break;
		}
	}
}

void TeslaManager::HandleMessage(const Message& msg)
{
	auto& cmd = msg.GetCmd();
	auto& id = msg.GetId();

	if (cmd == Cmd::Read && id == Id::VehicleInformation) {
		m_RuntimeMessageHandler.SendMessage(Message(Cmd::Answer, Id::VehicleInformation, m_VehicleInformation));
	}
	else if (cmd == Cmd::ReadWithDirectAnswer && id == Id::VehicleInformation) {
		msg.Answer(Message(Cmd::Answer, Id::VehicleInformation, m_VehicleInformation));
	}

}



