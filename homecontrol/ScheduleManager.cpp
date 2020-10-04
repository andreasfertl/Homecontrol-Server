#include "ScheduleManager.h"
#include "RuntimeId.h"
#include "IRuntimeConfigure.h"
#include "IRuntimeRegister.h"
#include "IGetConfiguration.h"
#include "Logging.h"
#include "MessageLightState.h"
#include <boost/date_time/posix_time/posix_time.hpp>

namespace {
	auto ConvertHoursToMyTimezone(long long hour, int offset) {
		return hour + offset;
	}
}


ScheduleManager::ScheduleManager(IPrint& iPrint, IConfigurationScheduleManager& iGetConfiguration, IRuntimeRegister& iRuntimeRegister, ISubscribe& iSubscribe) :
	m_IPrint(iPrint),
	m_RuntimeMessageHandler(iRuntimeRegister.RegisterRuntime({ StringTools::AsWstring(__FILE__), runtimeId::ScheduleManager, std::chrono::seconds(30) }, * this)),
	m_Schedules(iGetConfiguration.GetConfigurationScheduleManager().m_Schedules),
	m_UTCOffset(iGetConfiguration.GetConfigurationScheduleManager().m_UTCOffset)
{
}

ScheduleManager::~ScheduleManager()
{
	m_RuntimeMessageHandler.Stop();
}

void ScheduleManager::HandleLightSwitches(const std::vector<unsigned int>& ids, bool on)
{
	for (auto id : ids) {
		m_RuntimeMessageHandler.SendMessage(Message(Cmd::Write, Id::LightState, MessageLightState(id, on, L"")));
	}
}

void ScheduleManager::HandleSchedulerEvent(Schedule& schedule)
{
	if (!schedule.m_Handled) //not handled yet
	{
		if (schedule.m_ScheduleAction == ScheduleAction::lightswitchOn)
			HandleLightSwitches(schedule.m_InternalIds, true);
		else if (schedule.m_ScheduleAction == ScheduleAction::lightswitchOff)
			HandleLightSwitches(schedule.m_InternalIds, false);

		schedule.m_Handled = true;
	}
}

void ScheduleManager::Callback()
{
	auto date = boost::posix_time::microsec_clock::universal_time();
	auto now = date.time_of_day();

	for (auto& schedule : m_Schedules) {
		//check if it is time to fire this schedule?
		if (now.hours() == ConvertHoursToMyTimezone(schedule.m_StartHours, m_UTCOffset) && now.minutes() == schedule.m_StartMinutes) {
			HandleSchedulerEvent(schedule);
		}
		else {
			schedule.m_Handled = false;
		}
	}
}

void ScheduleManager::HandleMessage(const Message& msg)
{

}





