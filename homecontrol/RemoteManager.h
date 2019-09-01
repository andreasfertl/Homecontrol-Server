#pragma once 
#include <atomic>
#include <map>
#include "IRuntime.h"
#include "TCPHandler.h"
#include "iNetworkReceive.h"

struct CallbackTracking {

public:
	CallbackTracking() {};
	CallbackTracking(unsigned int id, std::function<void(const struct MessageLightState& msgLightState)> callback) : m_Id(id), m_Callback(callback) {};

	unsigned int m_Id;
	std::function<void(const struct MessageLightState& msgLightState)> m_Callback;
};

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
	std::map<unsigned int, CallbackTracking> m_CallbackTracker;
	TCPHandler m_TCPHandler;
	struct IRuntimeMessageHandling& m_RuntimeMessageHandler;
	struct ISubscribe& m_ISubscribe;
};


#pragma once
