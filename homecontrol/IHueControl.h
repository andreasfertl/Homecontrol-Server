#pragma once
#include "HueLightState.h"

struct IHueControl {
	virtual HueLightState ReadLightState(unsigned int lamp) = 0;
	virtual bool SetLightState(unsigned int lamp, HueLightState state) = 0;
};
