#pragma once
#include "Sensor.h"

struct IGetSensor {
	virtual Sensor GetSensor(int internalId) = 0;
};
