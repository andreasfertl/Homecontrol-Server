#include "RemoteTelldus.h"
#include "MessageLightState.h"


RemoteTelldus::RemoteTelldus(IRuntimeMessageHandling& iThreadMessageHandling) : m_IRuntimeMessageHandling(iThreadMessageHandling)
{
}

RemoteTelldus::~RemoteTelldus()
{
}

LightState RemoteTelldus::ReadLightState(unsigned int lamp)
{
	return LightState::Off; //not in use - can't read it back anyway 
}

bool RemoteTelldus::SetLightState(unsigned int lamp, LightState state)
{
	m_IRuntimeMessageHandling.SendMessage(Message(Cmd::Write, Id::TelldusDeviceSetState, MessageLightState(lamp, state == LightState::On ? true : false)));
	return true;
}

