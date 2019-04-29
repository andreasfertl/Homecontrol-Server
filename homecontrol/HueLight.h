#pragma once 
#include <string>
#include "HueLightState.h"
#include "IHueControl.h"

class HueLight
{
public:
	HueLight(IHueControl& iHueControl, unsigned int id, unsigned int lampId, const std::wstring& name) : m_IHueControl(iHueControl), m_Id(id), m_LampId(lampId), m_Name(name), m_State(iHueControl.ReadLightState(m_LampId)) {
	};

	unsigned int GetId() const { return m_Id; };
	const std::wstring& GetName() const { return m_Name; };
	HueLightState GetState() const { return m_State; };
	void UpdateState(HueLightState newState) { if (m_IHueControl.SetLightState(m_LampId, newState)) m_State = newState; };
	void ReadBackState() { m_State = m_IHueControl.ReadLightState(m_LampId); };

private:
	IHueControl& m_IHueControl;
	const unsigned int m_Id;
	const unsigned int m_LampId;
	const std::wstring m_Name;
	HueLightState m_State;
};


