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
	if (nwMessage.m_Message == "Get11\r\n")
		sensorId = 11;
	else if (nwMessage.m_Message == "Get12\r\n")
		sensorId = 12;
	else
		return;

	auto sensor = m_IJSONManager.getSensor(sensorId);

	//send answer back
	nwMessage.m_Answer(StringTools::AsString(sensor.serialize()));
}

void networkMessageHandler::Callback()
{
	//if i need to handle something cyclic
}

void networkMessageHandler::HandleMessage(const Message& msg)
{
}