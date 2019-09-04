#pragma once
#include <optional>
#include <map>
#include "IRuntime.h"
#include "ISubscribe.h"
#include "IGetConfiguration.h"
#include "IGetSensor.h"
#include "Sensor.h"


class SensorManager : public IRuntime, public IGetSensor
{
public:
	SensorManager(struct IPrint& iPrint, struct IConfigurationSensorManager& iGetConfiguration, struct IRuntimeRegister& iRuntimeRegister, ISubscribe& iSubscribe);
	~SensorManager();

	//IRuntime
	void Callback() override;
	void HandleMessage(const Message& msg) override;

	//IGetSensor 
	Sensor GetSensor(int id) override ;

private:
	struct IPrint& m_IPrint;
	struct IRuntimeMessageHandling& m_RuntimeMessageHandler;
	std::map<int, Sensor> m_Sensors;
};

