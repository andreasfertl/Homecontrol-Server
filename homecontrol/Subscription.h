#pragma once

#include <functional>
#include "IRuntimeMessageHandling.h"
#include "ISubscribe.h"

class Subscription : public ISubscribe
{
public:
	Subscription(struct IPrint& iPrint);
	~Subscription();

	//ISubscribe
	std::vector<std::reference_wrapper<IRuntimeMessageHandling>> FindSubscribers(Id id) const override;
	void Subscribe(const SubscriptionData&& subscription) override;

private:
	struct IPrint& m_IPrint;
	std::vector<SubscriptionData> m_Subscribers;

};


