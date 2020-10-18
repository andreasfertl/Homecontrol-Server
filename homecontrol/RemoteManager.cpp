#include "RemoteManager.h"
#include "RuntimeId.h"
#include "IRuntimeConfigure.h"
#include "ISubscribe.h"
#include "IRuntimeRegister.h"
#include "IGetConfiguration.h"
#include "StringTools.h"
#include "MessageLightState.h"
#include "Logging.h"


RemoteManager::RemoteManager(IPrint& iPrint, struct IConfigurationRemoteManager& iGetConfiguration, IRuntimeRegister& iRuntimeRegister, ISubscribe& iSubscribe) :
	m_IPrint(iPrint),
	m_TCPHandler(5005, *this, [this](std::wstring loggme) { Logg(m_IPrint, loggme); }),
	m_RuntimeMessageHandler(iRuntimeRegister.RegisterRuntime({ StringTools::AsWstring(__FILE__), runtimeId::RemoteManager, std::chrono::milliseconds(100) }, *this)),
	m_ISubscribe(iSubscribe)
{
	//maybe subscribe all?
	auto subscribeToIds = iGetConfiguration.GetConfigurationRemoteManager();
	Logg(m_IPrint, "Subscribing to (numberof): " << subscribeToIds.m_IdsToSubscribe.size());
	
	for (auto id : subscribeToIds.m_IdsToSubscribe)
	{
		Logg(m_IPrint, "Subscribe to: " << id);
		iSubscribe.Subscribe({ static_cast<Id>(id), m_RuntimeMessageHandler });
	}
}

RemoteManager::~RemoteManager()
{
}

void RemoteManager::Callback()
{
}

void RemoteManager::HandleMessage(const Message & msg)
{
	//i received a msg, i will just relay the message to all the remote peer(s)
	m_TCPHandler.sendToAllConnections(msg.AsJson());
}

void RemoteManager::receive(const networkMessage& nwMessage) const
{
	auto msg = Message(nwMessage.m_Message);

	Logg(m_IPrint, "Received Message: " << StringTools::AsWstring(nwMessage.m_Message));

	//if (msg.GetId() != Id::Alive)
//		Logg(m_IPrint, L"received: " << StringTools::AsWstring(nwMessage.m_Message));

	if (msg.GetCmd() == Cmd::Write && msg.GetId() == Id::Subscribe) //need to subscribe for remote peer 
	{
		Logg(m_IPrint, "Received subscribe Message");
		if (auto subscribeTo = msg.GetValue<int>(&m_IPrint))
			m_ISubscribe.Subscribe({ static_cast<Id>(*subscribeTo), m_RuntimeMessageHandler });
	}
	else {
		Logg(m_IPrint, "Forwarding message");
		m_RuntimeMessageHandler.SendMessage(msg); //just forward message
	}
}











