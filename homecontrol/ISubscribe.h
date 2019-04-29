#pragma once

#include <vector>
#include "MessageMetaData.h"
#include "IRuntimeMessageHandling.h"

struct SubscriptionData
{
public:
	SubscriptionData(Id id, std::reference_wrapper<IRuntimeMessageHandling> ithread) : m_Id(id), m_IRuntime(ithread) {}

	Id GetId() const { return m_Id; }
	std::reference_wrapper<IRuntimeMessageHandling> GetIRuntime() const { return m_IRuntime; }

	bool operator==(const SubscriptionData& rhs) const { return GetId() == rhs.GetId() && &GetIRuntime().get() == &rhs.GetIRuntime().get(); }

private:
	Id m_Id;
	std::reference_wrapper<IRuntimeMessageHandling> m_IRuntime;

};

struct ISubscribe {
	virtual std::vector<std::reference_wrapper<IRuntimeMessageHandling>> FindSubscribers(Id id) const = 0;
	virtual void Subscribe(const SubscriptionData&& subscription) = 0;
};
