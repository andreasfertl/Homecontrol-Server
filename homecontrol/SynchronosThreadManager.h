#pragma once
#include <list>
#include "IRuntimeRegister.h"
#include "IRuntimeMessageProcessing.h"
#include "ICallbacks.h"
#include "SynchronosThread.h"

class SynchronosThreadManager : public IRuntimeRegister, public IRuntimeMessageProcessing, public ICallbacks
{
public:
	SynchronosThreadManager(struct IPrint& iPrint);

	//IRuntimeRegister
	struct IRuntimeMessageHandling& RegisterRuntime(const struct IRuntimeConfigure& IRuntimeConfigure, struct IRuntime& iThread) override;

	//IRuntimeMessageProcessing
	void ProcessMessages(struct ISubscribe& iSubscribe) override;

	//ICallbacks
	void HandleCallbacks() override;

private:
	struct IPrint& m_IPrint;
	std::list<SynchronosThread> m_RuntimeManagers;
};

