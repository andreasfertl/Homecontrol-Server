#include "Configuration.h"
#include "IGetConfiguration.h"
#include "Logging.h"
#include "StringTools.h"
#include <cpprest/json.h>
#include <array>
#include <iostream>
#include <fstream>
#include <string>

namespace {

	std::wstring ReadConfigFile(const std::wstring& configurationFile)
	{
		std::wstring config;
		std::wstring line;
		
#ifdef _WIN32
		std::wifstream configfile(configurationFile);
#else 
		std::wifstream configfile(StringTools::AsString(configurationFile));
#endif 

		if (configfile.is_open())
		{
			while (getline(configfile, line))
				config.append(line);

			configfile.close();
		}

		return config;
	}

	std::optional<web::json::value> GetConfigSection(const std::wstring& section, const std::wstring& configFileContent)
	{
		try {

#ifdef _WIN32
			web::json::value config = web::json::value::parse(utility::conversions::to_string_t(configFileContent));
#else 
			web::json::value config = web::json::value::parse(StringTools::AsString(configFileContent));
#endif 
			//try reading the given section
#ifdef _WIN32
			auto confiSection = config[U("Configuration")][utility::conversions::to_string_t(section)];
#else 
			auto confiSection = config[U("Configuration")][StringTools::AsString(section)];
#endif 
			if (!confiSection.is_null())
				return confiSection;
		}
		catch(...) {
			return std::nullopt;
		}

		return std::nullopt;
	}


	ConfigurationSonos ReadSonosConfiguration(std::optional<web::json::value> configuration) {

		try {
			auto& sonosConf = configuration.value();

			if (!sonosConf.is_null())
			{
#ifdef _WIN32
				return ConfigurationSonos(
					sonosConf[U("secret")].as_string(),
					sonosConf[U("key")].as_string(),
					sonosConf[U("authUrl")].as_string(),
					sonosConf[U("tokenUrl")].as_string(),
					sonosConf[U("scope")].as_string(),
					sonosConf[U("redirectUrl")].as_string(),
					sonosConf[U("controlUrl")].as_string(),
					{
						sonosConf[U("configToken")][U("access_token")].as_string(),
						sonosConf[U("configToken")][U("refresh_token")].as_string(),
						sonosConf[U("configToken")][U("token_type")].as_string(),
						sonosConf[U("configToken")][U("scope")].as_string(),
					});
#else 
				return ConfigurationSonos(
					StringTools::AsWstring(sonosConf[U("secret")].as_string()),
					StringTools::AsWstring(sonosConf[U("key")].as_string()),
					StringTools::AsWstring(sonosConf[U("authUrl")].as_string()),
					StringTools::AsWstring(sonosConf[U("tokenUrl")].as_string()),
					StringTools::AsWstring(sonosConf[U("scope")].as_string()),
					StringTools::AsWstring(sonosConf[U("redirectUrl")].as_string()),
					StringTools::AsWstring(sonosConf[U("controlUrl")].as_string()),
					{
						StringTools::AsWstring(sonosConf[U("configToken")][U("access_token")].as_string()),
						StringTools::AsWstring(sonosConf[U("configToken")][U("refresh_token")].as_string()),
						StringTools::AsWstring(sonosConf[U("configToken")][U("token_type")].as_string()),
						StringTools::AsWstring(sonosConf[U("configToken")][U("scope")].as_string()),
					});
#endif 

			}
		}
		catch (...) {
		}

		//error....
		return ConfigurationSonos(L"", L"", L"", L"", L"", L"", L"", { L"", L"" , L"" , L"" }); //proceeding with a "empty" configuration
	}

	ConfigurationTeslaManager ReadTeslaConfiguration(std::optional<web::json::value> configuration) {

		try {
			auto& teslaConf = configuration.value();

			if (!teslaConf.is_null())
			{
				return ConfigurationTeslaManager(
					{
						StringTools::AsWstring(teslaConf[U("configToken")][U("access_token")].as_string()),
						StringTools::AsWstring(teslaConf[U("configToken")][U("refresh_token")].as_string()),
						StringTools::AsWstring(teslaConf[U("configToken")][U("token_type")].as_string()),
						StringTools::AsWstring(teslaConf[U("configToken")][U("scope")].as_string()),
					},
					StringTools::AsWstring(teslaConf[U("controlUrl")].as_string()),
					StringTools::AsWstring(teslaConf[U("vehicleId")].as_string()),
					teslaConf[U("retryWakeUps")].as_integer(),
					teslaConf[U("retrySleepInSeconds")].as_integer(),
					teslaConf[U("fastReadTimeInSeconds")].as_integer(),
					teslaConf[U("verySlowReadTimeInSeconds")].as_integer(),
					teslaConf[U("maxSlowingRateForVerySlowReads")].as_integer()
				);
			}
		}
		catch (...) {
		}

		//error....
		return ConfigurationTeslaManager({ L"", L"" , L"" , L"" }, L"", L"", 3, 30, 60, 1800, 3); //proceeding with a "empty" configuration
	}



	std::vector<ConfigurationLights> ReadConfiguredLights(std::optional<web::json::value>& config) {

		std::vector<ConfigurationLights> configuredLights;

		if (!config.has_value())
			return configuredLights;

		auto& lights = (*config)[U("Lights")];
		if (lights.is_array())
		{
			auto lightsArray = lights.as_array();
			for (auto light : lightsArray)
			{
#ifdef _WIN32
				configuredLights.emplace_back(light[U("InternalId")].as_integer(), light[U("id")].as_integer(), light[U("name")].as_string());
#else 
				configuredLights.emplace_back(light[U("InternalId")].as_integer(), light[U("id")].as_integer(), StringTools::AsWstring(light[U("name")].as_string()));
#endif 
			}
		}

		return configuredLights;
	}


	ConfigurationPhilipsHue ReadPhilipsHueConfiguration(std::optional<web::json::value> configuration, struct IPrint& iPrint) {

		try {
			if (!configuration.value().is_null())
			{
#ifdef _WIN32
				return ConfigurationPhilipsHue(
					configuration.value()[U("ip")].as_string(),
					configuration.value()[U("key")].as_string(),
					ReadConfiguredLights(configuration));
#else 
				auto ip = StringTools::AsWstring(configuration.value()[U("ip")].as_string());
				auto key = StringTools::AsWstring(configuration.value()[U("key")].as_string());
				Logg(iPrint, ip);
				Logg(iPrint, key);
				return ConfigurationPhilipsHue(ip, key, ReadConfiguredLights(configuration));
#endif 
			}
		}
		catch (...) {
		}

		//error....
		return ConfigurationPhilipsHue(L"", L"", std::vector<ConfigurationLights>()); //proceeding with a "empty" configuration
	}

	ConfigurationTelldus ReadTelldusConfiguration(std::optional<web::json::value> configuration) {

		try {
			return ConfigurationTelldus(ReadConfiguredLights(configuration));
		}
		catch (...) {
		}

		//error....
		return ConfigurationTelldus(std::vector<ConfigurationLights>()); //proceeding with a "empty" configuration
	}

	ConfigurationRemoteManager ReadRemoteManagerConfiguration(std::optional<web::json::value> configuration) {

		try {
			std::vector<unsigned int> remoteIdsToSubscribe;

			auto& ids = (*configuration)[U("ForwardIds")];
			if (ids.is_array())
			{
				auto idsArray = ids.as_array();
				for (auto id : idsArray)
				{
					auto idToPush = static_cast<unsigned int>(id[U("id")].as_integer());
					remoteIdsToSubscribe.push_back(idToPush);
				}
			}

			return ConfigurationRemoteManager(remoteIdsToSubscribe);
		}
		catch (...) {
		}

		//error....
		return ConfigurationRemoteManager(std::vector<unsigned int>()); //proceeding with a "empty" configuration
	}

	std::vector<TempHumidity> ReadConfiguredTempHumiditySensors(std::optional<web::json::value>& config) {

		std::vector<TempHumidity> configuredSensors;

		if (!config.has_value() || config->is_null())
			return configuredSensors;

		auto& sensors = (*config)[U("TempHumidity")];
		if (sensors.is_array())
		{
			auto sensorsArray = sensors.as_array();
			for (auto sensor : sensorsArray)
			{
#ifdef _WIN32
				configuredSensors.emplace_back(sensor[U("InternalId")].as_integer(), sensor[U("id")].as_integer(), sensor[U("name")].as_string());
#else 
				configuredSensors.emplace_back(sensor[U("InternalId")].as_integer(), sensor[U("id")].as_integer(), StringTools::AsWstring(sensor[U("name")].as_string()));
#endif 

			}
		}

		return configuredSensors;
	}


	ConfigurationSensorManager ReadSensorManagerConfigurationConfiguration(std::optional<web::json::value> configuration) {

		try {
			return ConfigurationSensorManager(ReadConfiguredTempHumiditySensors(configuration));
		}
		catch (...) {
		}

		//error....
		return ConfigurationSensorManager(std::vector<TempHumidity>()); //proceeding with a "empty" configuration
	}

	std::vector<Schedule> ReadConfiguredSchedules(std::optional<web::json::value>& config) {

		std::vector<Schedule> configuredSchedules;
		
		if (!config.has_value() || config->is_null())
			return configuredSchedules;

		auto& schedules = (*config)[U("Schedules")];
		if (schedules.is_array())
		{
			auto arr = schedules.as_array();
			for (auto schedule : arr)
			{
				std::vector<unsigned int> configuredIds;
				auto startHours = schedule[U("StartHour")].as_integer();
				auto startMinutes = schedule[U("StartMinute")].as_integer();
				auto scheduleAction = static_cast<ScheduleAction>(schedule[U("ScheduleAction")].as_integer());

				auto& internalIds = schedule[U("InternalIds")];
				if (internalIds.is_array()) {

					auto ids = internalIds.as_array();
					for (auto id : ids) {
						configuredIds.push_back(id[U("InternalId")].as_integer());
					}
				}
				configuredSchedules.emplace_back(startHours, startMinutes, scheduleAction, configuredIds);
			}
		}
		return configuredSchedules;
	}


	ConfigurationScheduleManager ReadScheduleManagerConfigurationConfiguration(std::optional<web::json::value> configuration) {

		try {
			return ConfigurationScheduleManager(ReadConfiguredSchedules(configuration), configuration.value()[U("UTCOffset")].as_integer());
		}
		catch (...) {
		}

		//error....
		return ConfigurationScheduleManager(std::vector<Schedule>(),0); //proceeding with a "empty" configuration
	}
}

class ImplConfiguration : public IGetConfiguration
{
public:
	ImplConfiguration(struct IPrint& iPrint, const std::wstring& configurationFile) :
		m_IPrint(iPrint),
		m_ConfigFileContent(ReadConfigFile(configurationFile)),
		m_ConfigurationSonos(ReadSonosConfiguration(GetConfigSection(L"Sonos", m_ConfigFileContent))),
		m_ConfigurationPhilipsHue(ReadPhilipsHueConfiguration(GetConfigSection(L"PhilipsHue", m_ConfigFileContent), m_IPrint)),
		m_ConfigurationTelldus(ReadTelldusConfiguration(GetConfigSection(L"Telldus", m_ConfigFileContent))),
		m_ConfigurationRemoteManager(ReadRemoteManagerConfiguration(GetConfigSection(L"RemoteManager", m_ConfigFileContent))),
		m_ConfigurationSensorManager(ReadSensorManagerConfigurationConfiguration(GetConfigSection(L"Sensors", m_ConfigFileContent))),
		m_ConfigurationScheduleManager(ReadScheduleManagerConfigurationConfiguration(GetConfigSection(L"Scheduler", m_ConfigFileContent))),
		m_ConfigurationTeslaManager(ReadTeslaConfiguration(GetConfigSection(L"Tesla", m_ConfigFileContent)))
	{
	}
	
	~ImplConfiguration()	{

	}

	const ConfigurationSonos& GetConfigurationSonos() override {
		return m_ConfigurationSonos;
	}

	const ConfigurationPhilipsHue& GetConfigurationPhilipsHue() override {
		return m_ConfigurationPhilipsHue;
	}

	const ConfigurationTelldus& GetConfigurationTelldus() override {
		return m_ConfigurationTelldus;
	}

	const ConfigurationRemoteManager& GetConfigurationRemoteManager() override {
		return m_ConfigurationRemoteManager;
	}

	const ConfigurationSensorManager& GetConfigurationSensorManager() override {
		return m_ConfigurationSensorManager;
	}

	const ConfigurationScheduleManager& GetConfigurationScheduleManager() override {
		return m_ConfigurationScheduleManager;
	}

	const ConfigurationTeslaManager& GetConfigurationTeslaManager() override {
		return m_ConfigurationTeslaManager;
	}
	

private:
	struct IPrint& m_IPrint;
	std::wstring m_ConfigFileContent;
	const ConfigurationSonos m_ConfigurationSonos;
	const ConfigurationPhilipsHue m_ConfigurationPhilipsHue;
	const ConfigurationTelldus m_ConfigurationTelldus;
	const ConfigurationRemoteManager m_ConfigurationRemoteManager;
	const ConfigurationSensorManager m_ConfigurationSensorManager;
	const ConfigurationScheduleManager m_ConfigurationScheduleManager;
	const ConfigurationTeslaManager m_ConfigurationTeslaManager;
};





Configuration::Configuration(struct IPrint& iPrint, const std::wstring& configurationFile) :
	m_ImplConfiguration(std::make_unique<ImplConfiguration>(iPrint, configurationFile))
{
}

Configuration::~Configuration()
{
}

IGetConfiguration& Configuration::IGetConfiguration() {
	return *m_ImplConfiguration;
}


