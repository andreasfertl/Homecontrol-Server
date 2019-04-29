#pragma once
#include <list>
#include <string>
#include "IRuntimeRegister.h"
#include "IRuntimeMessageProcessing.h"
#include "RuntimeId.h"
#include "Message.h"
#include <optional>
#include <chrono>
#include "Queue.h"
#include "IRuntimeInfo.h"
#include "IRuntimeMessageHandling.h"


class SynchronosThread : public IRuntimeInfo, public IRuntimeMessageHandling
{
public:
	SynchronosThread(struct IPrint& iPrint, const struct IRuntimeConfigure& IRuntimeConfigure, struct IRuntime& iThread);

	//IRuntimeInfo
	runtimeId GetRuntimeId() const override;
	const std::wstring& GetName() const override;

	//IRuntimeMessageHandling
	void SendMessage(const Message& msg) override;
	std::optional<Message> ReceiveMessageFromRuntimeHandler() override;
	void PushMessageToRuntimeHandler(const Message& msg) override;
	void Stop() {};

	void Callback();

private:
	struct IPrint& m_IPrint;
	const std::wstring  m_Name;          //the name of the "thread"
	const runtimeId     m_RuntimeId;     //unique
	struct IRuntime&    m_IRuntime;
	Queue<Message>		m_RxQueue;
	Queue<Message>		m_TxQueue;
	std::chrono::milliseconds m_RunTime;
	std::chrono::steady_clock::time_point m_StartTimer;
};