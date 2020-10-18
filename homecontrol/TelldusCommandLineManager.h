#pragma once
#include "IRuntime.h"
#include "ISubscribe.h"
#include "Light.h"




class TelldusCommandLineManager : public IRuntime, private ILightControl
{
public:
	TelldusCommandLineManager(struct IPrint& iPrint, struct IConfigurationTelldus& iGetConfiguration, struct IRuntimeRegister& iRuntimeRegister, ISubscribe& iSubscribe);
	~TelldusCommandLineManager();

	//IRuntime
	void Callback() override;
	void HandleMessage(const Message& msg) override;


	//ILightControl
	LightState ReadLightState(unsigned int lamp) override;
	bool SetLightState(unsigned int lamp, LightState state) override;

private:
	struct IPrint& m_IPrint;
	std::vector<Light> m_Lights;
	struct IRuntimeMessageHandling& m_RuntimeMessageHandler;
};

