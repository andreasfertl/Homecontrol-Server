#pragma once
#include <string>
#include <vector>
#include "IOauth2.h"

struct ConfigurationSonos {
public:
	ConfigurationSonos(const std::wstring& secret, const std::wstring& key, const std::wstring& authUrl, const std::wstring& tokenUrl, const std::wstring& scope, const std::wstring& redirectUrl, const std::wstring& controlUrl, const configToken& configToken) :
		m_Secret(secret),
		m_Key(key),
		m_AuthUrl(authUrl),
		m_TokenUrl(tokenUrl),
		m_Scope(scope),
		m_RedirectUrl(redirectUrl),
		m_ControlUrl(controlUrl),
		m_ConfigToken(configToken)
	{
	}

	const std::wstring m_Secret;
	const std::wstring m_Key;
	const std::wstring m_AuthUrl;
	const std::wstring m_TokenUrl;
	const std::wstring m_Scope;
	const std::wstring m_RedirectUrl;
	const std::wstring m_ControlUrl;
	const configToken m_ConfigToken;
};

struct ConfigurationLights {
public:
	ConfigurationLights(unsigned int internalId, unsigned int id, const std::wstring& name) :
		m_InternalId(internalId),
		m_Id(id),
		m_Name(name)
	{

	}

	const unsigned int m_InternalId;
	const unsigned int m_Id;
	const std::wstring m_Name;
};

struct ConfigurationPhilipsHue {
public:
	ConfigurationPhilipsHue(const std::wstring& ip, const std::wstring& key, const std::vector<ConfigurationLights>& lights) :
		m_Ip(ip),
		m_Key(key),
		m_Lights(lights)
	{
	}

	const std::wstring m_Ip;
	const std::wstring m_Key;
	const std::vector<ConfigurationLights> m_Lights;
};

struct ConfigurationTelldus {
public:
	ConfigurationTelldus(const std::vector<ConfigurationLights>& lights) :
		m_Lights(lights)
	{
	}

	const std::vector<ConfigurationLights> m_Lights;
};

struct ConfigurationRemoteManager {
public:
	ConfigurationRemoteManager(const std::vector<unsigned int>& idsToSubscribe) :
		m_IdsToSubscribe(idsToSubscribe)
	{
	}

	const std::vector<unsigned int> m_IdsToSubscribe;
};

struct TempHumidity {
public:
	TempHumidity(unsigned int internalId, unsigned int id, const std::wstring& name) :
		m_InternalId(internalId),
		m_Id(id),
		m_Name(name)
	{
	}

	const unsigned int m_InternalId;
	const int m_Id;
	const std::wstring m_Name;
};

struct ConfigurationSensorManager {
public:
	ConfigurationSensorManager(const std::vector<TempHumidity>& sensors) :
		m_TempHumiditySensors(sensors)
	{
	}

	const std::vector<TempHumidity> m_TempHumiditySensors;
};

enum class ScheduleAction {
	lightswitchOff = 0,
	lightswitchOn = 1,
};

struct Schedule {
public:
	Schedule(unsigned int startHours, unsigned int startMinutes, ScheduleAction scheduleAction, const std::vector<unsigned int>& internalIds) :
		m_StartHours(startHours),
		m_StartMinutes(startMinutes),
		m_ScheduleAction(scheduleAction),
		m_InternalIds(internalIds),
		m_Handled(false)
	{
	}

	const unsigned int m_StartHours;
	const unsigned int m_StartMinutes;
	const ScheduleAction m_ScheduleAction;
	const std::vector<unsigned int> m_InternalIds;
	bool m_Handled;
};

struct ConfigurationScheduleManager {
public:
	ConfigurationScheduleManager(const std::vector<Schedule>& schedules, int utcOffset) :
		m_Schedules(schedules),
		m_UTCOffset(utcOffset)
	{
	}

	const std::vector<Schedule> m_Schedules;
	const int m_UTCOffset;
};

struct ConfigurationTeslaManager {
public:
	ConfigurationTeslaManager(const configToken& configToken, const std::wstring& controlUrl, const std::wstring& vehicleId, unsigned int retryWakeUps, unsigned int retrySleepInSeconds, unsigned int fastReadTimeInSeconds, unsigned int verySlowReadTimeInSeconds, unsigned int maxSlowingRateForVerySlowReads)
		: m_ConfigToken(configToken)
		, m_controlUrl(controlUrl)
		, m_vehicleId(vehicleId)
		, m_retryWakeUps(retryWakeUps)
		, m_retrySleepInSeconds(retrySleepInSeconds)
		, m_fastReadTimeInSeconds(fastReadTimeInSeconds)
		, m_verySlowReadTimeInSeconds(verySlowReadTimeInSeconds)
		, m_maxSlowingRateForVerySlowReads(maxSlowingRateForVerySlowReads)
	{
	}

	const configToken m_ConfigToken;
	const std::wstring m_controlUrl;
	const std::wstring m_vehicleId;
	const unsigned int m_retryWakeUps;
	const unsigned int m_retrySleepInSeconds;
	const unsigned int m_fastReadTimeInSeconds;
	const unsigned int m_verySlowReadTimeInSeconds;
	const unsigned int m_maxSlowingRateForVerySlowReads;
};



struct IConfigurationSonos {
	virtual const ConfigurationSonos& GetConfigurationSonos() = 0;
};

struct IConfigurationTelldus {
	virtual const ConfigurationTelldus& GetConfigurationTelldus() = 0;
};

struct IConfigurationPhilipsHue {
	virtual const ConfigurationPhilipsHue& GetConfigurationPhilipsHue() = 0;
};

struct IConfigurationRemoteManager {
	virtual const ConfigurationRemoteManager& GetConfigurationRemoteManager() = 0;
};

struct IConfigurationSensorManager {
	virtual const ConfigurationSensorManager& GetConfigurationSensorManager() = 0;
};

struct IConfigurationScheduleManager {
	virtual const ConfigurationScheduleManager& GetConfigurationScheduleManager() = 0;
};

struct IConfigurationTeslaManager {
	virtual const ConfigurationTeslaManager& GetConfigurationTeslaManager() = 0;
};

struct IGetConfiguration : public IConfigurationSonos, public IConfigurationTelldus, public IConfigurationPhilipsHue, public IConfigurationRemoteManager, public IConfigurationSensorManager, public IConfigurationScheduleManager, public IConfigurationTeslaManager {

};