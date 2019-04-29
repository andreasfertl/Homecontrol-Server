#include "ThreadManager.h"
#include "IPrint.h"
#include "IRuntime.h"
#include "IRuntimeConfigure.h"
#include "ISubscribe.h"

ThreadManager::ThreadManager(IPrint& iPrint) :
	m_Threads()
{
}

struct IRuntimeMessageHandling& ThreadManager::RegisterRuntime(const struct IRuntimeConfigure& IRuntimeConfigure, struct IRuntime& iThread)
{
	return m_Threads.emplace_back(IRuntimeConfigure, iThread);
}


void ThreadManager::ProcessMessages(struct ISubscribe& iSubscribe)
{
	for (auto& thread : m_Threads) {
		//read all messages from this thread
		while (auto msg = thread.ReceiveMessageFromRuntimeHandler()) {
			auto subscribersToThisMessage = iSubscribe.FindSubscribers(msg->GetId());
			//send to all subscribers
			for (auto& threadToPostTo : subscribersToThisMessage)
				threadToPostTo.get().PushMessageToRuntimeHandler(*msg);
		}
	}
}


