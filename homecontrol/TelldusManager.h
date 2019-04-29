#pragma once 

#include <vector>
#include "IRuntime.h"
#include "RemoteTelldus.h"
#include "Light.h"

class TelldusManager : public IRuntime
{
public:
	TelldusManager(struct IPrint& iPrint, struct IGetConfiguration& iGetConfiguration, struct IRuntimeRegister& iRuntimeRegister, struct ISubscribe& iSubscribe);
	~TelldusManager();

	//IRuntime
	void Callback() override;
	void HandleMessage(const Message& msg) override;

private:
	struct IPrint& m_IPrint;
	std::vector<Light> m_Lights;
	struct IRuntimeMessageHandling& m_RuntimeMessageHandler;
	RemoteTelldus m_Remote;
};


