#pragma once
#include "IRuntime.h"
#include "ISubscribe.h"


class TelldusCommandLineManager : public IRuntime
{
public:
	TelldusCommandLineManager(struct IPrint& iPrint, struct IRuntimeRegister& iRuntimeRegister, ISubscribe& iSubscribe);
	~TelldusCommandLineManager();

	//IRuntime
	void Callback() override;
	void HandleMessage(const Message& msg) override;


private:
	struct IPrint& m_IPrint;
	struct IRuntimeMessageHandling& m_RuntimeMessageHandler;
};

