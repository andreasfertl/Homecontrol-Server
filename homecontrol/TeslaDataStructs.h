#pragma once

namespace TeslaDataStructs {

	inline double getKmFromMiles(double miles) {
		return miles*1.609344f;
	}

	struct vehicleState {
		vehicleState()
			: m_Locked(false)
			, m_OdometerInKm(0.0f)
			, m_fd_window(true)
		    , m_fp_window(true)
		    , m_rd_window(true)
		    , m_rp_window(true)
		    , m_df(true)  //driver front
		    , m_dr(true) //driver rear
		    , m_pf(true) //passenger front
		    , m_pr(true) //passenger rear
		    , m_ft(true) //front trunk
		    , m_rt(true) //rear trunk
		{};

		vehicleState(bool locked, double odometer, bool fd_window, bool fp_window, bool rd_window, bool rp_window, bool df, bool dr, bool pf, bool pr, bool ft, bool rt)
			: m_Locked(locked)
			, m_OdometerInKm(getKmFromMiles(odometer))
			, m_fd_window(fd_window)
			, m_fp_window(fp_window)
			, m_rd_window(rd_window)
			, m_rp_window(rp_window)
			, m_df(df)  //driver front
			, m_dr(dr) //driver rear
			, m_pf(pf) //passenger front
			, m_pr(pr) //passenger rear
			, m_ft(ft) //front trunk
			, m_rt(rt) //rear trunk
		{};

		bool m_Locked;
		double m_OdometerInKm;
		bool m_fd_window;
		bool m_fp_window;
		bool m_rd_window;
		bool m_rp_window;
		bool m_df;  //driver front
		bool m_dr; //driver rear
		bool m_pf; //passenger front
		bool m_pr; //passenger rear
		bool m_ft; //front trunk;
		bool m_rt; //rear trunk;
	};

	struct driveState {
		driveState()
			: m_heading(0u)
			, m_latitude(0.0f)
			, m_longitude(0.0f)
			, m_power(0)
			, m_speed(0)
		{};

		driveState(unsigned int heading, double latitude, double longitude, double power, double speed)
			: m_heading(heading)
			, m_latitude(latitude)
			, m_longitude(longitude)
			, m_power(power)
			, m_speed(speed)
		{};

		unsigned int m_heading;
		double m_latitude;
		double m_longitude;
		double m_power;
		double m_speed;
	};


	struct chargeState {
		chargeState()
			: m_Usable_battery_level(0u)
			, m_EstimatedRangeInKm(0.0f)
			, m_Charger_actual_current(0.0f)
			, m_Charger_actual_voltage(0)
			, m_Charging_state(L"Disconnected")
			, m_Time_to_full_charge(0.0f)
		{};

		chargeState(unsigned int usable_battery_level, double battery_rangeInMiles, double charger_actual_current, unsigned int charger_actual_voltage, const std::wstring& charging_state, double time_to_full_charge)
			: m_Usable_battery_level(usable_battery_level)
			, m_EstimatedRangeInKm(getKmFromMiles(battery_rangeInMiles))
			, m_Charger_actual_current(charger_actual_current)
			, m_Charger_actual_voltage(charger_actual_voltage)
			, m_Charging_state(charging_state)
			, m_Time_to_full_charge(time_to_full_charge)
		{};

		unsigned int m_Usable_battery_level;
		double m_EstimatedRangeInKm;
		double m_Charger_actual_current;
		unsigned int m_Charger_actual_voltage;
		std::wstring m_Charging_state;
		double m_Time_to_full_charge;
	};

	struct climateState {
		climateState()
			: m_Inside_temp(0.0f)
			, m_Outisde_temp(0.0f)
			, m_Driver_temp_setting(0.0f)
			, m_Passenger_temp_setting(0.0f)
			, m_Seat_heater_left(false)
			, m_Seat_heater_right(false)
		{};

		climateState(double insideTemp, double outisde_temp, double driver_temp_setting, double passenger_temp_setting, bool seat_heater_left, bool seat_heater_right)
			: m_Inside_temp(insideTemp)
			, m_Outisde_temp(outisde_temp)
			, m_Driver_temp_setting(driver_temp_setting)
			, m_Passenger_temp_setting(passenger_temp_setting)
			, m_Seat_heater_left(seat_heater_left)
			, m_Seat_heater_right(seat_heater_right)
		{};

		double m_Inside_temp;
		double m_Outisde_temp;
		double m_Driver_temp_setting;
		double m_Passenger_temp_setting;
		bool m_Seat_heater_left;
		bool m_Seat_heater_right;
	};

	struct vehicleInformation {

		vehicleInformation()
			: m_VehicleState()
			, m_DriveState()
			, m_ChargeState()
			, m_ClimateState()
		{};

		vehicleInformation(vehicleState vehicleState, driveState driveState, chargeState chargeState, climateState climateState)
			: m_VehicleState(vehicleState)
			, m_DriveState(driveState)
			, m_ChargeState(chargeState)
			, m_ClimateState(climateState)
		{};

		vehicleState m_VehicleState;
		driveState   m_DriveState;
		chargeState  m_ChargeState;
		climateState m_ClimateState;
	};
}
