#include "networkReceiveHandler.h"
#include "iNetworkReceive.h"
#include "Logging.h"
#include "StringTools.h"


netWorkReceiveHandler::netWorkReceiveHandler(IPrint& logger) :
	m_LoggManager(logger)
{
}

void netWorkReceiveHandler::receive(const networkMessage& nwMessage) const  {
	Logg(m_LoggManager, StringTools::AsWstring(nwMessage.m_Message));
	nwMessage.m_Answer(nwMessage.m_Message + "thanks");
}
