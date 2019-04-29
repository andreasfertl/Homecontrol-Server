#pragma once

struct IUpdateLightState
{
	virtual void UpdateLightState(unsigned int id, bool on) = 0;
};
