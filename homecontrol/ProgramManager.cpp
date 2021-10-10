#include "ProgramManager.h"
#include "IGetConfiguration.h"
#include "IPrint.h"
#include "iNetworkReceive.h"
#include "Logging.h"




ProgramManager::ProgramManager(IPrint& iPrint) :
	m_Configuration(iPrint, L"Configuration.json"),
	m_Run(true),
	m_Subscription(iPrint),
	m_ThreadManager(iPrint),
	//m_SynchronosThreadManager(iPrint),
	m_RemoteManager(iPrint, m_Configuration.IGetConfiguration(), m_ThreadManager, m_Subscription),
	//m_TelldusManager(iPrint, m_Configuration.IGetConfiguration(), m_ThreadManager, m_Subscription),
	m_HueManager(iPrint, m_Configuration.IGetConfiguration(), m_ThreadManager, m_Subscription),
	m_PresenceManager(iPrint, m_ThreadManager, m_Subscription),
	m_SensorManager(iPrint, m_Configuration.IGetConfiguration(), m_ThreadManager, m_Subscription),
	m_JSONManager(m_ThreadManager),
	m_RESTApi(m_JSONManager),
	m_HTTPManager(iPrint, m_ThreadManager, m_Subscription, m_RESTApi),
	m_HTTPServer(iPrint, L"http://192.168.68.130:40000/v1/", m_HTTPManager),
	//m_HTTPServer(iPrint, L"http://172.21.74.9:40000/v1/", m_RESTApi),
	m_NetworkReceiveHandler(iPrint),
	m_NetworkMessageHandler(iPrint, m_ThreadManager, m_Subscription, m_JSONManager),
	m_TCPHandler(30000, m_NetworkReceiveHandler, [&iPrint](std::wstring loggme) { Logg(iPrint, loggme); }),
	m_CommandLineManager(iPrint, m_ThreadManager, m_Subscription),
	m_TelldusCommandLineManager(iPrint, m_Configuration.IGetConfiguration(), m_ThreadManager, m_Subscription),
	m_ScheduleManager(iPrint, m_Configuration.IGetConfiguration(), m_ThreadManager, m_Subscription)
	//m_TeslaManager(iPrint, m_Configuration.IGetConfiguration(), m_ThreadManager, m_Subscription)
	//m_SonosManager(iPrint, m_Configuration.IGetConfiguration(), m_ThreadManager, m_Subscription)
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

