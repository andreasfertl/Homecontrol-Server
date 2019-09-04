#pragma once
#include "IRuntimeMessageHandling.h"
#include <future>

namespace MassageWithDirectAnswer { 
	template<typename T, typename U>
	T SendAndRead(IRuntimeMessageHandling& iRuntime, Id id, U data) {
		std::promise<T> value;
		std::future<T> valueFuture = value.get_future();

		std::function<void(Message)> answerCallback = [&value](Message answMessage) {
			if (auto rxData = answMessage.GetValue<U>()) {
				value.set_value(*rxData);
			}
			else {
				value.set_value({});
			}
		};

		//sending message
		iRuntime.SendMessage(Message(Cmd::ReadWithDirectAnswer, id, data, answerCallback));

		//waiting for answer
		valueFuture.wait();
		return valueFuture.get();
	}
}