#pragma once

#include <optional>
#include "Message.h"

struct IRuntimeMessageHandling {
	virtual void SendMessage(const Message& msg) = 0;
	virtual std::optional<Message> ReceiveMessageFromRuntimeHandler() = 0;
	virtual void PushMessageToRuntimeHandler(const Message & msg) = 0;
	virtual void Stop() = 0;
};

