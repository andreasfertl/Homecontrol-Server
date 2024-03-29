#pragma once 

#include <atomic>
#include "Subscription.h"
#include "TelldusManager.h"
#include "HueManager.h"
#include "RemoteManager.h"
#include "PresenceManager.h"
#include "SonosManager.h"
#include "ThreadManager.h"
#include "SynchronosThreadManager.h"
#include "Configuration.h"
#include "TCPHandler.h"
#include "networkReceiveHandler.h"
#include "networkMessageHandler.h"
#include "JSONManager.h"
#include "HTTPServer.h"
#include "RESTApi.h"
#include "SensorManager.h"
#include "CommandLineManager.h"
#include "TelldusCommandLineManager.h"
#include "ScheduleManager.h"
#include "TeslaManager.h"
#include "HTTPManager.h"

class ProgramManager
{
public:
	ProgramManager(struct IPrint& iPrint);

	void Run();
	void Stop();

private:
	Configuration			m_Configuration;
	std::atomic<bool>       m_Run;
	Subscription            m_Subscription;
	ThreadManager           m_ThreadManager;
	//SynchronosThreadManager m_SynchronosThreadManager;
	RemoteManager	        m_RemoteManager;
	//TelldusManager          m_TelldusManager;
	HueManager		        m_HueManager;
	PresenceManager         m_PresenceManager;
	SensorManager           m_SensorManager;
	JSONManager             m_JSONManager;
	RESTApi                 m_RESTApi;
	HTTPManager             m_HTTPManager;
	HTTPServer				m_HTTPServer;
	netWorkReceiveHandler   m_NetworkReceiveHandler;
	networkMessageHandler   m_NetworkMessageHandler;
	TCPHandler				m_TCPHandler;
	CommandLineManager      m_CommandLineManager;
	TelldusCommandLineManager m_TelldusCommandLineManager;
	ScheduleManager           m_ScheduleManager;
	//TeslaManager              m_TeslaManager;
	//SonosManager            m_SonosManager;
};

