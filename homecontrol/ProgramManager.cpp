#include "ProgramManager.h"
#include "IGetConfiguration.h"
#include "IPrint.h"


ProgramManager::ProgramManager(IPrint& iPrint) :
	m_Configuration(iPrint, L"Configuration.json"),
	m_Run(true),
	m_Subscription(iPrint),
	m_ThreadManager(iPrint),
	//m_SynchronosThreadManager(iPrint),
	m_RemoteManager(iPrint, m_Configuration.IGetConfiguration(), m_ThreadManager, m_Subscription),
	m_TelldusManager(iPrint, m_Configuration.IGetConfiguration(), m_ThreadManager, m_Subscription),
	m_HueManager(iPrint, m_Configuration.IGetConfiguration(), m_ThreadManager, m_Subscription),
	m_MappingManager(iPrint, m_ThreadManager, m_Subscription),
	m_SonosManager(iPrint, m_Configuration.IGetConfiguration(), m_ThreadManager, m_Subscription)
{
}


void ProgramManager::Run()
{
	while (m_Run) {
		m_ThreadManager.ProcessMessages(m_Subscription);
		m_ThreadManager.HandleCallbacks();

		std::this_thread::sleep_for(std::chrono::milliseconds(1));
	}
}

void ProgramManager::Stop()
{
	m_Run = false;
}

