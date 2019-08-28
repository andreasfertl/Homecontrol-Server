#pragma once
#include "iNetworkReceive.h"
#include "IRuntime.h"

class networkMessageHandler : public iNetworkReceive, public IRuntime
{
public:
	networkMessageHandler(struct IPrint& iPrint, struct IRuntimeRegister& iRuntimeRegister, struct ISubscribe& iSubscribe);

	//iNetworkReceive
	void receive(const networkMessage& nwMessage) const override;

	//IRuntime
	void Callback() override;
	void HandleMessage(const Message& msg) override;

private:
	struct IPrint& m_IPrint;
	struct IRuntimeMessageHandling& m_RuntimeMessageHandler;

	double m_TempSensor11Cache;
	double m_TempSensor12Cache;
};


