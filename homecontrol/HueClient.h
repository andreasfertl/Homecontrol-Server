#pragma once
#include <string>
#include <vector>
#include "Light.h"
#include "ILightControl.h"

class HueClient : public ILightControl {

public:
	HueClient(const std::wstring& ip, const std::wstring& key);

	//ILightControl
	LightState ReadLightState(unsigned int lamp);
	bool SetLightState(unsigned int lamp, LightState state);

private:
	const std::wstring m_Ip;
	const std::wstring m_Key;

};