#pragma once
#include "IRuntime.h"
#include "ISubscribe.h"


class CommandLineManager : public IRuntime
{
public:
	CommandLineManager(struct IPrint& iPrint, struct IRuntimeRegister& iRuntimeRegister, ISubscribe& iSubscribe);
	~CommandLineManager();

	//IRuntime
	void Callback() override;
	void HandleMessage(const Message& msg) override;


private:
	struct IPrint& m_IPrint;
	struct IRuntimeMessageHandling& m_RuntimeMessageHandler;
};

