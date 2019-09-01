#pragma once 
#include <atomic>
#include <map>
#include "IRuntime.h"
#include "TCPHandler.h"
#include "iNetworkReceive.h"

class RemoteManager : public IRuntime, private iNetworkReceive
{
public:
	RemoteManager(struct IPrint& iPrint, struct IConfigurationRemoteManager& iGetConfiguration, struct IRuntimeRegister& iRuntimeRegister, struct ISubscribe& iSubscribe);
	~RemoteManager();

	//IRuntime
	void Callback() override;
	void HandleMessage(const Message& msg) override;

	//iNetworkReceive
	void receive(const networkMessage& nwMessage) const override;

private:
	struct IPrint& m_IPrint;
	TCPHandler m_TCPHandler;
	struct IRuntimeMessageHandling& m_RuntimeMessageHandler;
	struct ISubscribe& m_ISubscribe;
};


#pragma once
