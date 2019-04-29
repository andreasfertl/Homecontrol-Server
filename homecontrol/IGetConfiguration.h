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

struct IGetConfiguration {
	virtual const ConfigurationSonos& GetConfigurationSonos() = 0;
	virtual const ConfigurationPhilipsHue& GetConfigurationPhilipsHue() = 0;
	virtual const ConfigurationTelldus& GetConfigurationTelldus() = 0;
	virtual const ConfigurationRemoteManager& GetConfigurationRemoteManager() = 0;
};