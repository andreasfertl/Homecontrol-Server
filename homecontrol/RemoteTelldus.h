#pragma once 
#include <string>
#include "ILightControl.h"
#include "IRuntimeMessageHandling.h"

class RemoteTelldus : public ILightControl
{
public:
	RemoteTelldus(IRuntimeMessageHandling& iThreadMessageHandling);
	~RemoteTelldus();

	//ILightControl
	LightState ReadLightState(unsigned int lamp);
	bool SetLightState(unsigned int lamp, LightState state);

private:
	IRuntimeMessageHandling& m_IRuntimeMessageHandling;
};


