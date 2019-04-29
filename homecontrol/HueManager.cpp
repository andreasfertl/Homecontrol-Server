#include "StringTools.h"
#include "RuntimeId.h"
#include "HueManager.h"
#include "IRuntimeConfigure.h"
#include "IRuntimeRegister.h"
#include "MessageLightState.h"
#include "Logging.h"

namespace {
	void AddLight(IPrint& iPrint, std::vector<Light>& lights, ILightControl& ILightControl, unsigned int id, unsigned int lampId, const std::wstring& name)
	{
		lights.emplace_back(ILightControl, id, lampId, name);
		Logg(iPrint, L"Name: " << name << L" ID: " << id << L" internal ID: " << lampId);
	}

	std::vector<Light> AddLights(IPrint& iPrint, struct IGetConfiguration& iGetConfiguration, HueClient& hueClient)
	{
		std::vector<Light> lights;

		Logg(iPrint, L"Adding PhilipsHueLights:");
		for (auto& lightToAdd : iGetConfiguration.GetConfigurationPhilipsHue().m_Lights)
			AddLight(iPrint, lights, hueClient, lightToAdd.m_InternalId, lightToAdd.m_Id, lightToAdd.m_Name);

		return lights;
	}

}
HueManager::HueManager(IPrint& iPrint, struct IGetConfiguration& iGetConfiguration, IRuntimeRegister& iRuntimeRegister, ISubscribe& iSubscribe) :
	m_IPrint(iPrint),
	m_RuntimeMessageHandler(iRuntimeRegister.RegisterRuntime({ StringTools::AsWstring(__FILE__), runtimeId::HueManager, std::chrono::milliseconds(100) }, *this)),
	m_HueClient(iGetConfiguration.GetConfigurationPhilipsHue().m_Ip, iGetConfiguration.GetConfigurationPhilipsHue().m_Key),
	m_Lights(AddLights(iPrint, iGetConfiguration, m_HueClient))
{
	//configuration
	iSubscribe.Subscribe({ Id::LightState, m_RuntimeMessageHandler });
}

HueManager::~HueManager()
{
	m_RuntimeMessageHandler.Stop();
}

void HueManager::Callback()
{
	//read back state of lamps cyclic
	//std::for_each(std::begin(m_Lights), std::end(m_Lights), [](auto& Light) {
	//	Light.ReadBackState();
	//});

}

std::vector<Light>::iterator HueManager::GetLamp(unsigned int lampId)
{
	return std::find_if(std::begin(m_Lights), std::end(m_Lights), [lampId](const Light& lamp) { return lampId == lamp.GetId(); });
}

void HueManager::SetLamp(unsigned int lampId, bool state)
{
	auto stateToUpdateTo = state ? LightState::On : LightState::Off;
	auto lamp = GetLamp(lampId);

	if (lamp != m_Lights.end())
		lamp->UpdateState(stateToUpdateTo);
}


void HueManager::HandleMessage(const Message & msg)
{
	if (msg.GetId() == Id::LightState)
	{
		if (auto lightstate = msg.GetValue<MessageLightState>(&m_IPrint))
		{
			const auto& cmd = msg.GetCmd();
			if (cmd == Cmd::Write)
			{
				SetLamp(lightstate->m_Id, lightstate->m_On);
			}
			else if (cmd == Cmd::Read)
			{
				auto lamp = GetLamp(lightstate->m_Id);
				if (lamp != m_Lights.end())
					m_RuntimeMessageHandler.SendMessage(Message(Cmd::Answer, Id::LightState, MessageLightState(lamp->GetId(), lamp->GetState() == LightState::On ? true : false)));
			}
		}
	}
}










