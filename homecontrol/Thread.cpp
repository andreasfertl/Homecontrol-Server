#include "Thread.h"
#include "IRuntime.h"
#include "IRuntimeConfigure.h"
#include "PreparseMessage.h"


Thread::Thread(const IRuntimeConfigure& IRuntimeConfigure, IRuntime& iThread) :
	m_Event(),
	m_Run(true),
	m_Name(IRuntimeConfigure.name),
	m_RuntimeId(IRuntimeConfigure.RuntimeId),
	m_IRuntime(iThread),
	m_RxQueue(),
	m_TxQueue(),
	m_MessageHandlerThread(std::bind(&Thread::HandleNewMessage, this)),
	m_Timer(IRuntimeConfigure.runTime, std::bind(&Thread::TimerCallback, this))
{
}

Thread::~Thread()
{
	std::function<void()> callback = [this]() {
		m_Run = false;
	};

	//transfer call to our thread and ensure with sending a message that our Message Handler thread will awake
	PushMessageToRuntimeHandler(Message(Cmd::Run, Id::Run, callback));
}

void Thread::Stop()
{
	m_Timer.Stop();
}


runtimeId Thread::GetRuntimeId() const
{
	return m_RuntimeId;
}

const std::wstring& Thread::GetName() const
{
	return m_Name; 
}

void Thread::SendMessage(const Message & msg)
{
	m_TxQueue.Push(msg);
}

std::optional<Message> Thread::ReceiveMessageFromRuntimeHandler()
{
	return m_TxQueue.Pop();
}

void Thread::PushMessageToRuntimeHandler(const Message& msg)
{
	{
		std::lock_guard<std::mutex> lk(m_Event.m_Mutex);
		m_RxQueue.Push(msg);
	}
	m_Event.m_Condition.notify_one();
}

void Thread::TimerCallback()
{
	std::function<void()> callback = [this]() {	
		//just call our thread owner
		this->m_IRuntime.Callback();
	};

	//transfer call to same context as our message handler
	PushMessageToRuntimeHandler(Message(Cmd::Run, Id::Run, callback));
}

void Thread::HandleNewMessage()
{
	while (m_Run)
	{
		//waiting for event
		std::unique_lock<std::mutex> lk(m_Event.m_Mutex);
		m_Event.m_Condition.wait(lk, [this]() { return this->m_RxQueue.Size() > 0; });

		//clear the queue
		while (auto msg = m_RxQueue.Pop())
		{
			if (msg.has_value())
			{
				auto& msgReceived = msg.value();
				if (PreParseMsg::PreParseMsg(msgReceived) != PreParseMsg::preParseMsg::handeld)
					m_IRuntime.HandleMessage(msgReceived);
			}
		}
	}
}







