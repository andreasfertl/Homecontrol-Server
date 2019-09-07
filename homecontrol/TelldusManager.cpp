#include "StringTools.h"
#include "RuntimeId.h"
#include "TelldusManager.h"
#include "IRuntimeConfigure.h"
#include "ISubscribe.h"
#include "IRuntimeMessageHandling.h"
#include "IGetConfiguration.h"
#include "IRuntimeRegister.h"
#include "MessageLightState.h"
#include "RemoteTelldus.h"
#include "Logging.h"

namespace {

	void AddLight(IPrint& iPrint, std::vector<Light>& lights, ILightControl& ILightControl, unsigned int id, unsigned int lampId, const std::wstring& name)
	{
		lights.emplace_back(ILightControl, id, lampId, name);
		Logg(iPrint, L"Name: " << name << L" ID: " << id << L" internal ID: " << lampId);
	}

}

TelldusManager::TelldusManager(IPrint& iPrint, struct IConfigurationTelldus& iGetConfiguration, IRuntimeRegister& iRuntimeRegister, ISubscribe& iSubscribe) :
	m_IPrint(iPrint),
	m_Lights(),
	m_RuntimeMessageHandler(iRuntimeRegister.RegisterRuntime({ StringTools::AsWstring(__FILE__), runtimeId::TelldusManager, std::chrono::milliseconds(100) }, *this)),
	m_Remote(m_RuntimeMessageHandler)
{
	iSubscribe.Subscribe({ Id::LightState, m_RuntimeMessageHandler });
	iSubscribe.Subscribe({ Id::ArctechSwitch, m_RuntimeMessageHandler });

	//Interface / generalID / TelldusLampId, name, state
	Logg(m_IPrint, L"Adding telldusLights:");
	auto lights = iGetConfiguration.GetConfigurationTelldus();
	for (auto& light : lights.m_Lights)
		AddLight(m_IPrint, m_Lights, m_Remote, light.m_InternalId, light.m_Id, light.m_Name);
}


TelldusManager::~TelldusManager()
{
}

void TelldusManager::Callback()
{
}


void TelldusManager::HandleMessage(const Message & msg)
{
	if (msg.GetId() == Id::LightState)
	{
		if (auto lightstate = msg.GetValue<MessageLightState>(&m_IPrint))
		{
			const auto& cmd = msg.GetCmd();
			if (cmd == Cmd::Write)
			{
				for (auto& lamp : m_Lights) {
					if (lightstate->m_Id == lamp.GetId())
						lamp.UpdateState(lightstate->m_On ? LightState::On : LightState::Off);
				};
			}
			else if (cmd == Cmd::Read)
			{
				for (auto& lamp : m_Lights) {
					if (lightstate->m_Id == lamp.GetId())
						m_RuntimeMessageHandler.SendMessage(Message(Cmd::Answer, Id::LightState, MessageLightState(lamp.GetId(), lamp.GetState() == LightState::On ? true : false, L"")));
				};
			}
			else if (cmd == Cmd::ReadWithDirectAnswer) {
				for (auto& lamp : m_Lights) {
					if (lightstate->m_Id == lamp.GetId()) {
						msg.Answer(Message(Cmd::Answer, Id::LightState, MessageLightState(lamp.GetId(), lamp.GetState() == LightState::On ? true : false, L"")));
					}
				};
			}
		}
	}
}











