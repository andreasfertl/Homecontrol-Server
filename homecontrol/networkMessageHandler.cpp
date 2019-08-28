#include "networkMessageHandler.h"
#include "StringTools.h"
#include "Message.h"
#include "IRuntimeConfigure.h"
#include "ISubscribe.h"
#include "IRuntimeRegister.h"
#include "Logging.h"
#include "MandloynSensor.h"
#include <chrono>
#include <future>


networkMessageHandler::networkMessageHandler(IPrint& iPrint, IRuntimeRegister& iRuntimeRegister, ISubscribe& iSubscribe) :
	m_IPrint(iPrint),
	m_RuntimeMessageHandler(iRuntimeRegister.RegisterRuntime({ StringTools::AsWstring(__FILE__), runtimeId::NetworkMessageHandler, std::chrono::milliseconds(100) }, *this)),
	m_TempSensor11Cache(0.0f),
	m_TempSensor12Cache(12.34f)
{
	iSubscribe.Subscribe({ Id::MandolynSensor, m_RuntimeMessageHandler });
}

void networkMessageHandler::receive(const networkMessage& nwMessage) const
{
	Logg(m_IPrint, L"received: " + StringTools::AsWstring(nwMessage.m_Message));
	
	//parsing CMD
	int sensorId(0);
	if (nwMessage.m_Message == "Get11\r\n")
		sensorId = 11;
	else if (nwMessage.m_Message == "Get12\r\n")
		sensorId = 12;
	else
		return;

	//setting up lambda & reading of value
	std::promise<double> sensorValue;
	std::future<double> sensorValueFuture = sensorValue.get_future();

	std::function<void()> readSensor = [this, sensorId, &sensorValue]() {
		if (sensorId == 11)
			sensorValue.set_value(this->m_TempSensor11Cache);
		else if (sensorId == 12)
			sensorValue.set_value(this->m_TempSensor12Cache);
	};

	//sending message
	m_RuntimeMessageHandler.PushMessageToRuntimeHandler(Message(Cmd::Run, Id::Run, readSensor));
	
	//waiting for answer
	sensorValueFuture.wait();
	auto sensorValueAnswer = sensorValueFuture.get();
	//format answer
	std::ostringstream answerMessage;
	answerMessage << "SensorID: " << sensorId << " Value: " << sensorValueAnswer;

	//send answer back
	nwMessage.m_Answer(answerMessage.str());
}

void networkMessageHandler::Callback()
{
	//if i need to handle something cyclic
}

void networkMessageHandler::HandleMessage(const Message& msg)
{
	if (msg.GetId() == Id::MandolynSensor)
	{
		if (auto sensor = msg.GetValue<MandolynSensor>(&m_IPrint))
		{
			const auto& cmd = msg.GetCmd();
			if (cmd == Cmd::Write) //receiving temperature
			{
				auto id = sensor->GetId();
				if (id == 11)
					m_TempSensor11Cache = sensor->GetTemp();
				else if (id == 12)
					m_TempSensor12Cache = sensor->GetTemp();
			}

		}
	}

}