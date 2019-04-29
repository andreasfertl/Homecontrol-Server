#pragma once
#include "LightState.h"

struct ILightControl {
	virtual LightState ReadLightState(unsigned int lamp) = 0;
	virtual bool SetLightState(unsigned int lamp, LightState state) = 0;
};
