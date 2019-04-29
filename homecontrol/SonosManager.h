#pragma once 
#include "IRuntime.h"
#include "ISubscribe.h"
#include "SonosConnection.h"
#include "SonosClient.h"
#include "OAuth2.h"

class SonosManager : public IRuntime
{
public:
	SonosManager(struct IPrint& iPrint, struct IGetConfiguration& iGetConfiguration, struct IRuntimeRegister& iRuntimeRegister, ISubscribe& iSubscribe);
	~SonosManager();

	//IRuntime
	void Callback() override;
	void HandleMessage(const Message& msg) override;

private:
	struct IPrint& m_IPrint;
	const struct ConfigurationSonos& m_Configuration;
	OAuth2 m_Oauth;
	SonosConnection m_SonosConnection;
	SonosClient m_SonosClient;
	struct IRuntimeMessageHandling& m_RuntimeMessageHandler;
};


