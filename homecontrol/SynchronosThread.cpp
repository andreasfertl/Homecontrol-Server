#include "SynchronosThread.h"
#include "PreparseMessage.h"
#include "Logging.h"
#include "IRuntime.h"
#include "IRuntimeConfigure.h"


SynchronosThread::SynchronosThread(struct IPrint& iPrint, const IRuntimeConfigure& IRuntimeConfigure, IRuntime& iThread) :
	m_IPrint(iPrint),
	m_Name(IRuntimeConfigure.name),
	m_RuntimeId(IRuntimeConfigure.RuntimeId),
	m_IRuntime(iThread),
	m_RxQueue(),
	m_TxQueue(),
	m_RunTime(IRuntimeConfigure.runTime),
	m_StartTimer(std::chrono::steady_clock::now())
{
}


runtimeId SynchronosThread::GetRuntimeId() const
{
	return m_RuntimeId;
}

const std::wstring& SynchronosThread::GetName() const
{
	return m_Name;
}

void SynchronosThread::SendMessage(const Message & msg)
{
	m_TxQueue.Push(msg);
}

std::optional<Message> SynchronosThread::ReceiveMessageFromRuntimeHandler()
{
	return m_TxQueue.Pop();
}

void SynchronosThread::PushMessageToRuntimeHandler(const Message& msg)
{
	m_RxQueue.Push(msg);
}

void SynchronosThread::Callback()
{
	if (std::chrono::steady_clock::now() - m_StartTimer >= m_RunTime)
	{
		m_StartTimer = std::chrono::steady_clock::now();

		while (auto msg = m_RxQueue.Pop())
		{
			if (msg.has_value())
			{
				auto& msgReceived = msg.value();
				if (PreParseMsg::PreParseMsg(msgReceived) != PreParseMsg::preParseMsg::handeld)
					m_IRuntime.HandleMessage(msgReceived);
			}
		}

		//call our thread owner
		m_IRuntime.Callback();
	}
}
