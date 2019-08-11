#include "RemoteManager.h"
#include "RuntimeId.h"
#include "IRuntimeConfigure.h"
#include "ISubscribe.h"
#include "IRuntimeRegister.h"
#include "IGetConfiguration.h"
#include "StringTools.h"
#include "TCPServer.h"
#include "MessageLightState.h"
#include "Logging.h"



RemoteManager::RemoteManager(IPrint& iPrint, struct IGetConfiguration& iGetConfiguration, IRuntimeRegister& iRuntimeRegister, ISubscribe& iSubscribe) :
	m_IPrint(iPrint),
	m_CallbackTracker(),
	m_SendQueue(),
	m_TCPServer(5005, std::bind(&RemoteManager::ReceiveMessage, this, std::placeholders::_1), this->m_SendQueue),
	m_TCPServer2(5006, std::bind(&RemoteManager::ReceiveMessage, this, std::placeholders::_1), this->m_SendQueue), 
	m_RuntimeMessageHandler(iRuntimeRegister.RegisterRuntime({ StringTools::AsWstring(__FILE__), runtimeId::RemoteManager, std::chrono::milliseconds(100) }, *this)),
	m_ISubscribe(iSubscribe)
{
	//maybe subscribe all?
	auto subscribeToIds = iGetConfiguration.GetConfigurationRemoteManager();
	for (auto id : subscribeToIds.m_IdsToSubscribe)
	{
		iSubscribe.Subscribe({ static_cast<Id>(id), m_RuntimeMessageHandler });
	}
}

RemoteManager::~RemoteManager()
{
}


void RemoteManager::ReceiveMessage(const std::string& message)
{
	auto msg = Message(message);
	if (msg.GetCmd() == Cmd::Write && msg.GetId() == Id::Subscribe)
	{
		if (auto subscribeTo = msg.GetValue<int>(&m_IPrint))
			m_ISubscribe.Subscribe({ static_cast<Id>(*subscribeTo), m_RuntimeMessageHandler });
	}
	else {
		m_RuntimeMessageHandler.SendMessage(msg);
	}
}

void RemoteManager::Callback()
{
}

void RemoteManager::HandleMessage(const Message & msg)
{
	//i received a msg, i will just relay the message to the remote peer
	m_SendQueue.Push(msg.AsJson());
}











