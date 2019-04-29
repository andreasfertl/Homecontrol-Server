#pragma once
#include <list>
#include "IRuntimeRegister.h"
#include "IRuntimeMessageProcessing.h"
#include "ICallbacks.h"
#include "Thread.h"

class ThreadManager: public IRuntimeRegister, public IRuntimeMessageProcessing, public ICallbacks
{
public:
	ThreadManager(struct IPrint& iPrint);
	
	//IRuntimeRegister
	struct IRuntimeMessageHandling& RegisterRuntime(const struct IRuntimeConfigure& IRuntimeConfigure, struct IRuntime& iThread) override;

	//IRuntimeMessageProcessing
	void ProcessMessages(struct ISubscribe& iSubscribe) override;

	//ICallbacks
	void HandleCallbacks() override {}; //nothing to do, will be handled asynchronosly by the threads themselv


private:
	std::list<Thread> m_Threads; //should be protect? lock/queue? - add with RegisterRuntime is done in constructor only and ProcessMessages only in runtime
};

