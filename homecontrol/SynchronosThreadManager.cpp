#include "SynchronosThreadManager.h"
#include "IPrint.h"
#include "IRuntime.h"
#include "IRuntimeConfigure.h"
#include "ISubscribe.h"
#include "PreparseMessage.h"
#include "IRuntime.h"
#include "IRuntimeConfigure.h"
#include "ICallbacks.h"

SynchronosThreadManager::SynchronosThreadManager(IPrint& iPrint) :
	m_IPrint(iPrint),
	m_RuntimeManagers()
{
}


struct IRuntimeMessageHandling& SynchronosThreadManager::RegisterRuntime(const struct IRuntimeConfigure& IRuntimeConfigure, struct IRuntime& iThread)
{
	return m_RuntimeManagers.emplace_back(m_IPrint, IRuntimeConfigure, iThread);
}


void SynchronosThreadManager::ProcessMessages(struct ISubscribe& iSubscribe)
{
	for (auto& runtimeManager : m_RuntimeManagers) {
		//read all messages from this thread
		while (auto msg = runtimeManager.ReceiveMessageFromRuntimeHandler()) {
			auto subscribersToThisMessage = iSubscribe.FindSubscribers(msg->GetId());
			//send to all subscribers
			for (auto& runtimeManagerToPostTo : subscribersToThisMessage)
				runtimeManagerToPostTo.get().PushMessageToRuntimeHandler(*msg);
		}
	}
}

void SynchronosThreadManager::HandleCallbacks()
{
	for (auto& synchronosThread : m_RuntimeManagers) {
		synchronosThread.Callback();
	}
}


