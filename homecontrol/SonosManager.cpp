#include "SonosManager.h"
#include "IRuntimeConfigure.h"
#include "IRuntimeRegister.h"
#include "Logging.h"
#include "OAuth2.h"
#include "IGetConfiguration.h"

#include <cpprest/http_client.h>

using namespace utility;
using namespace web;
using namespace web::http;
using namespace web::http::client;

SonosManager::SonosManager(IPrint& iPrint, struct IGetConfiguration& iGetConfiguration, IRuntimeRegister& iRuntimeRegister, ISubscribe& iSubscribe) :
	m_IPrint(iPrint),
	m_Configuration(iGetConfiguration.GetConfigurationSonos()),
	m_Oauth(m_IPrint, m_Configuration.m_Secret, m_Configuration.m_Key, m_Configuration.m_AuthUrl, m_Configuration.m_TokenUrl, m_Configuration.m_Scope, m_Configuration.m_RedirectUrl),
	m_SonosConnection(iPrint, m_Oauth, m_Configuration.m_ControlUrl),
	m_SonosClient(iPrint, m_SonosConnection),
	m_RuntimeMessageHandler(iRuntimeRegister.RegisterRuntime({ StringTools::AsWstring(__FILE__), runtimeId::SonosManager, std::chrono::milliseconds(100) }, *this))
{
	//configuration
	iSubscribe.Subscribe({ Id::SoundPause, m_RuntimeMessageHandler });
	iSubscribe.Subscribe({ Id::SoundPlay, m_RuntimeMessageHandler });

	if (m_Configuration.m_ConfigToken.access_token != L"")
	{
		m_Oauth.SetTokenConfig(m_Configuration.m_ConfigToken);
	}
	else //i don't have any credentials, request them from the user
	{
		m_Oauth.Authenticate();
	}
}

SonosManager::~SonosManager()
{
	m_RuntimeMessageHandler.Stop();
}

void SonosManager::Callback()
{
}

void SonosManager::HandleMessage(const Message & msg)
{
	if (msg.GetCmd() != Cmd::Write)
		return;

	auto& id = msg.GetId();
	if (id == Id::SoundPause)
	{
		m_SonosClient.Pause();
		Logg(m_IPrint, L"received Sonos Pause");
	}
	else if (id == Id::SoundPlay)
	{
		m_SonosClient.Play();
		Logg(m_IPrint, L"received Sonos Play");
	}
}










