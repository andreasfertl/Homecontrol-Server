#pragma once 

#include <atomic>
#include "Subscription.h"
#include "TelldusManager.h"
#include "HueManager.h"
#include "RemoteManager.h"
#include "MappingManager.h"
#include "SonosManager.h"
#include "ThreadManager.h"
#include "SynchronosThreadManager.h"
#include "Configuration.h"

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
	TelldusManager          m_TelldusManager;
	HueManager		        m_HueManager;
	MappingManager          m_MappingManager;
	//SonosManager            m_SonosManager;
};

