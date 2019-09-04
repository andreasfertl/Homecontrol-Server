#include "SensorManager.h"
#include "RuntimeId.h"
#include "IRuntimeConfigure.h"
#include "IRuntimeRegister.h"
#include "IGetConfiguration.h"
#include "MandloynSensor.h"
#include "Logging.h"

SensorManager::SensorManager(IPrint& iPrint, IConfigurationSensorManager& iGetConfiguration, IRuntimeRegister& iRuntimeRegister, ISubscribe& iSubscribe) :
	m_IPrint(iPrint),
	m_RuntimeMessageHandler(iRuntimeRegister.RegisterRuntime({ StringTools::AsWstring(__FILE__), runtimeId::SensorManager, std::chrono::milliseconds(100) }, * this)),
	m_Sensors()
{
	iSubscribe.Subscribe({ Id::MandolynSensor, m_RuntimeMessageHandler });
}

SensorManager::~SensorManager()
{
}

void SensorManager::Callback()
{
}

void SensorManager::HandleMessage(const Message& msg)
{
	const auto& cmd = msg.GetCmd();
	if (msg.GetId() == Id::MandolynSensor)
	{
		if (cmd == Cmd::Write) {
			if (auto sensor = msg.GetValue<MandolynSensor>(&m_IPrint)) {
				auto id = sensor->GetId();
				m_Sensors[id] = Sensor(id, sensor->GetTemp(), sensor->GetHumidity());
			}
		}
	}
	else if (msg.GetId() == Id::Sensor) {
		if (cmd == Cmd::Read) {
			if (auto sensorToRead = msg.GetValue<Sensor>(&m_IPrint))
			{
				m_RuntimeMessageHandler.SendMessage(Message(Cmd::Answer, Id::Sensor, GetSensor(sensorToRead->m_Id)));
			}
		}
	}
}

Sensor SensorManager::GetSensor(int id) {
	auto it = m_Sensors.find(id);
	if (it != std::end(m_Sensors))
		return it->second;
	else
		return Sensor();
}
