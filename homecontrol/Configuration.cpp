#include "Configuration.h"
#include "IGetConfiguration.h"
#include "Logging.h"
#include <cpprest/json.h>
#include <array>

#include <iostream>
#include <fstream>

namespace {

	std::wstring ReadConfigFile(const std::wstring& configurationFile)
	{
		std::wstring config;
		std::wstring line;
		
		std::wifstream configfile(configurationFile);

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
			web::json::value config = web::json::value::parse(utility::conversions::to_string_t(configFileContent));

			//try reading the given section
			auto confiSection = config[U("Configuration")][utility::conversions::to_string_t(section)];
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
			}
		}
		catch (...) {
		}

		//error....
		return ConfigurationSonos(L"", L"", L"", L"", L"", L"", L"", { L"", L"" , L"" , L"" }); //proceeding with a "empty" configuration
	}


	std::vector<ConfigurationLights> ReadConfiguredLights(std::optional<web::json::value>& config) {

		std::vector<ConfigurationLights> configuredLights;

		if (config->is_null())
			return configuredLights;

		auto& lights = (*config)[U("Lights")];
		if (lights.is_array())
		{
			auto lightsArray = lights.as_array();
			for (auto light : lightsArray)
			{
				configuredLights.emplace_back(light[U("InternalId")].as_integer(), light[U("id")].as_integer(), light[U("name")].as_string());
			}
		}

		return configuredLights;
	}


	ConfigurationPhilipsHue ReadPhilipsHueConfiguration(std::optional<web::json::value> configuration) {

		try {
			if (!configuration.value().is_null())
			{
				return ConfigurationPhilipsHue(
					configuration.value()[U("ip")].as_string(),
					configuration.value()[U("key")].as_string(),
					ReadConfiguredLights(configuration));
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



}

class ImplConfiguration : public IGetConfiguration
{
public:
	ImplConfiguration(struct IPrint& iPrint, const std::wstring& configurationFile) :
		m_IPrint(iPrint),
		m_ConfigFileContent(ReadConfigFile(configurationFile)),
		m_ConfigurationSonos(ReadSonosConfiguration(GetConfigSection(L"Sonos", m_ConfigFileContent))),
		m_ConfigurationPhilipsHue(ReadPhilipsHueConfiguration(GetConfigSection(L"PhilipsHue", m_ConfigFileContent))),
		m_ConfigurationTelldus(ReadTelldusConfiguration(GetConfigSection(L"Telldus", m_ConfigFileContent))),
		m_ConfigurationRemoteManager(ReadRemoteManagerConfiguration(GetConfigSection(L"RemoteManager", m_ConfigFileContent))),
		m_ConfigurationSensorManager()
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

private:
	struct IPrint& m_IPrint;
	std::wstring m_ConfigFileContent;
	const ConfigurationSonos m_ConfigurationSonos;
	const ConfigurationPhilipsHue m_ConfigurationPhilipsHue;
	const ConfigurationTelldus m_ConfigurationTelldus;
	const ConfigurationRemoteManager m_ConfigurationRemoteManager;
	const ConfigurationSensorManager m_ConfigurationSensorManager;
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


