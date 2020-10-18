#include "RuntimeId.h"
#include "PresenceManager.h"
#include "IRuntimeConfigure.h"
#include "ISubscribe.h"
#include "IRuntimeMessageHandling.h"
#include "IRuntimeRegister.h"
#include "BluetoothDevice.h"
#include "Logging.h"


namespace {

	bool isAtLeastSomeoneAtHome(std::map<int, BluetoothDevice>& presence, const BluetoothDevice& btDev) {
		//just update
		presence[btDev.GetDeviceId()] = btDev;

		//still at least someone at home?
		for (const auto& someoneAtHome : presence) {
			const auto& device = someoneAtHome.second;
			if (device.GetInRange()) {
				return true;
			}
		}

		return false;
	}
}

PresenceManager::PresenceManager(IPrint& iPrint, IRuntimeRegister& iRuntimeRegister, ISubscribe& iSubscribe) :
	m_IPrint(iPrint),
	m_RuntimeMessageHandler(iRuntimeRegister.RegisterRuntime({ StringTools::AsWstring(__FILE__), runtimeId::PresenceManager, std::chrono::milliseconds(1000) }, *this)),
	m_SomeOneAtHome(),
	m_AtLeastOneAtHome(false)
{
	iSubscribe.Subscribe({ Id::BlootoothDevice, m_RuntimeMessageHandler });
	iSubscribe.Subscribe({ Id::Presence, m_RuntimeMessageHandler });
}


void PresenceManager::Callback()
{
}


void PresenceManager::HandleMessage(const Message& msg)
{
	auto& cmd = msg.GetCmd();
	auto& id = msg.GetId();
	Logg(m_IPrint, "received message");
	if (cmd == Cmd::Write && id == Id::BlootoothDevice)
	{
		Logg(m_IPrint, "received BT Device");
		if (auto btDev = msg.GetValue<BluetoothDevice>(&m_IPrint))
		{
			Logg(m_IPrint, btDev->ToString());
			auto wasSomeOneAtHomeEarlier = m_AtLeastOneAtHome;
			m_AtLeastOneAtHome = isAtLeastSomeoneAtHome(m_SomeOneAtHome, *btDev);


			if (wasSomeOneAtHomeEarlier == false && m_AtLeastOneAtHome == true) {
				Logg(m_IPrint, L"Change: Someone came home");
			}
			else if (wasSomeOneAtHomeEarlier == true && m_AtLeastOneAtHome == false) {
				Logg(m_IPrint, L"Change: The last person left");
			}
			else if (wasSomeOneAtHomeEarlier == true && m_AtLeastOneAtHome == true) {
				;//"no" change - at least someone at home 
			}
			else if (wasSomeOneAtHomeEarlier == false && m_AtLeastOneAtHome == false) {
				;//"no" change - NO one at home 
			}
		}
	}
	else if (cmd == Cmd::Read && id == Id::Presence) {
		Logg(m_IPrint, "received read Presence");
		m_RuntimeMessageHandler.SendMessage(Message(Cmd::Answer, Id::Presence, m_AtLeastOneAtHome));
	}
	else if (cmd == Cmd::ReadWithDirectAnswer && id == Id::Presence) {
		Logg(m_IPrint, "received read Presence with direct answer");
		msg.Answer(Message(Cmd::Answer, Id::Presence, m_AtLeastOneAtHome));
	}
}


