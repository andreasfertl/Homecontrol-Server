#include "networkMessageHandler.h"
#include "StringTools.h"
#include "Message.h"
#include "IRuntimeConfigure.h"
#include "ISubscribe.h"
#include "IRuntimeRegister.h"
#include "Logging.h"
#include "MandloynSensor.h"
#include "Sensor.h"
#include "IJSONManager.h"

networkMessageHandler::networkMessageHandler(IPrint& iPrint, IRuntimeRegister& iRuntimeRegister, ISubscribe& iSubscribe, struct IJSONManager& iJSONManager) :
	m_IPrint(iPrint),
	m_IJSONManager(iJSONManager),
	m_RuntimeMessageHandler(iRuntimeRegister.RegisterRuntime({ StringTools::AsWstring(__FILE__), runtimeId::NetworkMessageHandler, std::chrono::milliseconds(100) }, *this))
{
	iSubscribe.Subscribe({ Id::MandolynSensor, m_RuntimeMessageHandler });
}

void networkMessageHandler::receive(const networkMessage& nwMessage) const
{
	//parsing CMD
	int sensorId(0);
	if (nwMessage.m_Message == "Get1\r\n")
		sensorId = 1030;
	else if (nwMessage.m_Message == "Get2\r\n")
		sensorId = 1031;
	else
		return;

	auto sensor = m_IJSONManager.getSensor(sensorId);

	//send answer back
	nwMessage.m_Answer(utility::conversions::to_utf8string(sensor.serialize()));
}

void networkMessageHandler::Callback()
{
	//if i need to handle something cyclic
}

void networkMessageHandler::HandleMessage(const Message& msg)
{
}