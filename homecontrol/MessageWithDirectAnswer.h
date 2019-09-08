#pragma once
#include "IRuntimeMessageHandling.h"
#include <future>

namespace MassageWithDirectAnswer { 
	template<typename T>
	T SendAndRead(IRuntimeMessageHandling& iRuntime, Id id, T data) {
		std::promise<T> value;
		std::future<T> valueFuture = value.get_future();
		
		std::function<void(Message)> answerCallback = [&value](Message answMessage) {
			if (auto rxData = answMessage.GetValue<T>()) {
				value.set_value(*rxData);
			}
		};

		//sending message
		iRuntime.SendMessage(Message(Cmd::ReadWithDirectAnswer, id, data, answerCallback));

		//waiting for answer
		auto status = valueFuture.wait_for(std::chrono::milliseconds(500));
		if (status == std::future_status::ready)
			return valueFuture.get();
		else
			return {};
	}
}