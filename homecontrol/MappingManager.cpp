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
#include <boost/date_time/posix_time/posix_time.hpp>

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
		iSender.SendMessage(Message(Cmd::Write, Id::LightState, MessageLightState(1023, on, L"")));
		iSender.SendMessage(Message(Cmd::Write, Id::LightState, MessageLightState(1024, on, L"")));
	}
		
	auto ConvertHoursToMyTimezone(long long hour) {
		if (hour > 0)
			return hour - 1; //!!! need to read the timezone
		return hour;
	}

	auto constructTime() {

		std::string ts("2019-12-08 02:57:59.000");
		boost::posix_time::ptime time(boost::posix_time::time_from_string(ts));
		return time;
	}

	auto simulateTime() {
		static boost::posix_time::ptime timeDate;
		static bool init(true);
		if (init) {
			timeDate = constructTime();
			init = false;
		}

		//increment seconds
		//timeDate += boost::posix_time::seconds(1);
		timeDate += boost::posix_time::hours(1);

		return timeDate.time_of_day();
	}
}


MappingManager::MappingManager(IPrint& iPrint, IRuntimeRegister& iRuntimeRegister, ISubscribe& iSubscribe) :
	m_IPrint(iPrint),
	m_RuntimeMessageHandler(iRuntimeRegister.RegisterRuntime({ StringTools::AsWstring(__FILE__), runtimeId::MappingManager, std::chrono::milliseconds(1000) }, *this)),
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
	auto date = boost::posix_time::microsec_clock::universal_time();
	//auto time = date.time_of_day();
	auto time = simulateTime();

	switch (state)
	{
	case 0:
		Logg(m_IPrint, L"Starting day at: " << StringTools::AsWstring(to_simple_string(date.date())) << L" " << StringTools::AsWstring(to_simple_string(time)));
		state = 1;
		break;

	case 1:
		if (time.hours() >= ConvertHoursToMyTimezone(15))
		{
			//lights on
			SendAutomaticLights(m_RuntimeMessageHandler, true);
			Logg(m_IPrint, L"Lights ON at: " << StringTools::AsWstring(to_simple_string(time)));
			state = 2;
		}
		break;

	case 2:
		if (time.hours() >= ConvertHoursToMyTimezone(23))
		{
			//lights off
			SendAutomaticLights(m_RuntimeMessageHandler, false);
			Logg(m_IPrint, L"Lights OFF at: " << StringTools::AsWstring(to_simple_string(time)));
			state = 3;
		}
		break;

	case 3:
		//wait to next day to go further automatic
		if (time.hours() == ConvertHoursToMyTimezone(0))
		{
			Logg(m_IPrint, L"Shifting day at: " << StringTools::AsWstring(to_simple_string(date.date())) << L" " << StringTools::AsWstring(to_simple_string(time)));
			state = 1;
		}
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
					//SendAutomaticLights(m_RuntimeMessageHandler, m_AtLeastOneAtHome);
				}
			}
		}

	}

}











