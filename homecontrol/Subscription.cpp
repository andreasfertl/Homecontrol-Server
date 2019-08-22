#include <algorithm>
#include "Subscription.h"
#include "Logging.h"

Subscription::Subscription(struct IPrint& iPrint) :
	m_IPrint(iPrint),
	m_Subscribers()
{
}

Subscription::~Subscription()
{
}

void Subscription::Subscribe(const SubscriptionData&& subscription) 
{
	//TBD needs to be thread safe!!!
	Logg(m_IPrint, L"Subscribing to: " << static_cast<int>(subscription.GetId()));
	if (std::find(m_Subscribers.begin(), m_Subscribers.end(), subscription) == m_Subscribers.end())
		m_Subscribers.emplace_back(subscription);
}

std::vector<std::reference_wrapper<IRuntimeMessageHandling>> Subscription::FindSubscribers(Id id) const
{
	std::vector<std::reference_wrapper<IRuntimeMessageHandling>> subscribers;

	for (const auto& element : m_Subscribers) {
		if (element.GetId() == id)
			subscribers.emplace_back(element.GetIRuntime());
	};

	return subscribers;
}
