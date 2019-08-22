#pragma once 
#include <optional>
#include "IRuntime.h"
#include "ISubscribe.h"
#include "Light.h"
#include "HueClient.h"
#include "IGetConfiguration.h"


class HueManager : public IRuntime
{
public:
	HueManager(struct IPrint& iPrint, struct IConfigurationPhilipsHue& iGetConfiguration, struct IRuntimeRegister& iRuntimeRegister, ISubscribe& iSubscribe);
	~HueManager();

	//IRuntime
	void Callback() override;
	void HandleMessage(const Message& msg) override;

private:
	void SetLamp(unsigned int lampId, bool state);
	std::vector<Light>::iterator GetLamp(unsigned int lampId);

	struct IPrint& m_IPrint;
	struct IRuntimeMessageHandling& m_RuntimeMessageHandler;
	HueClient m_HueClient;
	std::vector<Light> m_Lights;

};


