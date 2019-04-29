#pragma once 

#include <optional>
#include "Timer.h"
#include "RuntimeId.h"
#include "Queue.h"
#include "IRuntimeInfo.h"
#include "IRuntimeMessageHandling.h"


class Thread : public IRuntimeInfo, public IRuntimeMessageHandling
{
	public:
		Thread(const struct IRuntimeConfigure& IRuntimeConfigure, struct IRuntime& iThread);
		~Thread();

		//IRuntimeInfo
		runtimeId GetRuntimeId() const override;
		const std::wstring& GetName() const override;

		//IRuntimeMessageHandling
		void SendMessage(const Message& msg) override;
		std::optional<Message> ReceiveMessageFromRuntimeHandler() override;
		void PushMessageToRuntimeHandler(const Message& msg) override;
		void Stop() override;

	private:
		void HandleNewMessage();
		void TimerCallback();

		struct Event {
			std::mutex m_Mutex;
			std::condition_variable m_Condition;
		};

		Event				m_Event;
		bool                m_Run;
		const std::wstring  m_Name;         //the name of the thread
		const runtimeId     m_RuntimeId;     //unique
		struct IRuntime&    m_IRuntime;
		Queue<Message>		m_RxQueue;
		Queue<Message>		m_TxQueue;
		std::thread			m_MessageHandlerThread;
		Timer				m_Timer;
};


