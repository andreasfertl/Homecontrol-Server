#include "StringTools.h"
#include "RuntimeId.h"
#include "MappingManager.h"
#include "IRuntimeConfigure.h"
#include "ISubscribe.h"
#include "IRuntimeMessageHandling.h"
#include "IRuntimeRegister.h"
#include "ArctechSwitch.h"
#include "MessageLightState.h"
#include "BluetoothDevice.h"
#include "Logging.h"
#include "MandloynSensor.h"

namespace {

	void SendAutomaticLights(IRuntimeMessageHandling& iSender, bool on)
	{
		iSender.SendMessage(Message(Cmd::Write, Id::LightState, MessageLightState(1004, on, L"")));
		iSender.SendMessage(Message(Cmd::Write, Id::LightState, MessageLightState(1005, on, L"")));
		iSender.SendMessage(Message(Cmd::Write, Id::LightState, MessageLightState(1006, on, L"")));
		iSender.SendMessage(Message(Cmd::Write, Id::LightState, MessageLightState(1007, on, L"")));
		iSender.SendMessage(Message(Cmd::Write, Id::LightState, MessageLightState(1009, on, L"")));

		iSender.SendMessage(Message(Cmd::Write, Id::LightState, MessageLightState(1012, on, L"")));
		iSender.SendMessage(Message(Cmd::Write, Id::LightState, MessageLightState(1013, on, L"")));
		iSender.SendMessage(Message(Cmd::Write, Id::LightState, MessageLightState(1014, on, L"")));
		iSender.SendMessage(Message(Cmd::Write, Id::LightState, MessageLightState(1015, on, L"")));
		iSender.SendMessage(Message(Cmd::Write, Id::LightState, MessageLightState(1016, on, L"")));
	}
}


MappingManager::MappingManager(IPrint& iPrint, IRuntimeRegister& iRuntimeRegister, ISubscribe& iSubscribe) :
	m_IPrint(iPrint),
	m_RuntimeMessageHandler(iRuntimeRegister.RegisterRuntime({ StringTools::AsWstring(__FILE__), runtimeId::MappingManager, std::chrono::milliseconds(100) }, *this)),
	m_SomeOneAtHome(),
	m_AtLeastOneAtHome(false)
{
	iSubscribe.Subscribe({ Id::ArctechSwitch, m_RuntimeMessageHandler });
	iSubscribe.Subscribe({ Id::BlootoothDevice, m_RuntimeMessageHandler });
}


MappingManager::~MappingManager()
{
}


static unsigned int state(0);

void MappingManager::Callback()
{
	static float temperatureInc(0.0f);

	switch (state)
	{
	case 0:
		temperatureInc = temperatureInc + 0.02f;
		if (temperatureInc > 20)
			temperatureInc = 0.02f;
		std::this_thread::sleep_for(std::chrono::seconds(1));
		state = 1;
		break;
	case 1:
		/*m_Thread.SendMessage(Message(Cmd::Write, Id::LightState, MessageLightState(1004, true)));
		m_Thread.SendMessage(Message(Cmd::Write, Id::LightState, MessageLightState(1005, true)));
		m_Thread.SendMessage(Message(Cmd::Write, Id::LightState, MessageLightState(1006, true)));
		m_Thread.SendMessage(Message(Cmd::Write, Id::LightState, MessageLightState(1007, true)));
		m_Thread.SendMessage(Message(Cmd::Write, Id::LightState, MessageLightState(1009, true)));

		m_Thread.SendMessage(Message(Cmd::Write, Id::LightState, MessageLightState(1012, true)));
		m_Thread.SendMessage(Message(Cmd::Write, Id::LightState, MessageLightState(1013, true)));
		m_Thread.SendMessage(Message(Cmd::Write, Id::LightState, MessageLightState(1014, true)));
		m_Thread.SendMessage(Message(Cmd::Write, Id::LightState, MessageLightState(1015, true)));
		m_Thread.SendMessage(Message(Cmd::Write, Id::LightState, MessageLightState(1016, true)));*/
		//m_RuntimeMessageHandler.SendMessage(Message(Cmd::Write, Id::SoundPause));
		m_RuntimeMessageHandler.SendMessage(Message(Cmd::Write, Id::MandolynSensor, MandolynSensor(21, (18.4f+temperatureInc), 56)));
		m_RuntimeMessageHandler.SendMessage(Message(Cmd::Write, Id::MandolynSensor, MandolynSensor(11, (17.2f+temperatureInc), 75)));
		m_RuntimeMessageHandler.SendMessage(Message(Cmd::Write, Id::MandolynSensor, MandolynSensor(12, (22.1f+temperatureInc), 41)));
		state = 0;
		break;

	default:
		break;

	}

}


void MappingManager::HandleMessage(const Message & msg)
{
	if (msg.GetCmd() == Cmd::Write && msg.GetId() == Id::ArctechSwitch)
	{
		if (auto val = msg.GetValue<ArctechSwitch>(&m_IPrint))
		{
			auto& artechswitch = *val;
			auto housecode = artechswitch.GetHouse();
			auto group = artechswitch.GetGroup();
			auto unit = artechswitch.GetUnit();
			auto on = artechswitch.GetOn();

			if (housecode == 7733322)
			{
				switch (unit)
				{
				case 1:
					if (group != 0) //than it its group
						;
					else //single click
						m_RuntimeMessageHandler.SendMessage(Message(Cmd::Write, Id::LightState, MessageLightState(1010, on, L"")));
					break;
				case 2:
					if (on)
						m_RuntimeMessageHandler.SendMessage(Message(Cmd::Write, Id::SoundPlay));
					else
						m_RuntimeMessageHandler.SendMessage(Message(Cmd::Write, Id::SoundPause));
					break;
				case 3:
					m_RuntimeMessageHandler.SendMessage(Message(Cmd::Write, Id::LightState, MessageLightState(1001, on, L"")));
					break;
				}
			}
			else if (housecode == 12881742)
			{
				SendAutomaticLights(m_RuntimeMessageHandler, on);
			}
			else if (housecode == 15617150)
			{
				m_RuntimeMessageHandler.SendMessage(Message(Cmd::Write, Id::LightState, MessageLightState(1001, on, L"")));
			}
			else if (housecode == 17756142)
			{
				switch (unit)
				{
				case 11:
					m_RuntimeMessageHandler.SendMessage(Message(Cmd::Write, Id::LightState, MessageLightState(1011, on, L"")));
					break;
				case 12:
					m_RuntimeMessageHandler.SendMessage(Message(Cmd::Write, Id::LightState, MessageLightState(1001, on, L"")));
					break;
				}
			}
		}
	}
	else if (msg.GetCmd() == Cmd::Write && msg.GetId() == Id::BlootoothDevice)
	{
		if (auto btDev = msg.GetValue<BluetoothDevice>(&m_IPrint))
		{
			auto oldSomeOneAtHome = m_SomeOneAtHome;
			m_SomeOneAtHome[btDev->GetAddress()] = btDev->GetInRange();

			//did something change?
			if (oldSomeOneAtHome != m_SomeOneAtHome)
			{
				//still someone at home?
				bool atLeastOneAtHome = false;
				for (const auto& someoneAtHome : m_SomeOneAtHome)
				{
					const bool& atHome = someoneAtHome.second;
					if (atHome)
					{
						atLeastOneAtHome = true;
						break;
					}
				}

				//mapping at least one at home to on/off
				if (m_AtLeastOneAtHome != atLeastOneAtHome)
				{
					m_AtLeastOneAtHome = atLeastOneAtHome;
					SendAutomaticLights(m_RuntimeMessageHandler, m_AtLeastOneAtHome);
				}
			}
		}

	}

}











