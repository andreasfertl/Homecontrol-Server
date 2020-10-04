#pragma once
#include <vector>
#include "IRuntime.h"
#include "ISubscribe.h"
#include "IGetConfiguration.h"


class ScheduleManager : public IRuntime
{
public:
	ScheduleManager(struct IPrint& iPrint, struct IConfigurationScheduleManager& iGetConfiguration, struct IRuntimeRegister& iRuntimeRegister, ISubscribe& iSubscribe);
	~ScheduleManager();

	//IRuntime
	void Callback() override;
	void HandleMessage(const Message& msg) override;

private:
	void HandleSchedulerEvent(Schedule& schedule);
	void HandleLightSwitches(const std::vector<unsigned int>& ids, bool on);


	struct IPrint& m_IPrint;
	struct IRuntimeMessageHandling& m_RuntimeMessageHandler;
	std::vector<Schedule> m_Schedules;
	const int m_UTCOffset;
};

