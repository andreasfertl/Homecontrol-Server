#pragma once 
#include <string>
#include "LightState.h"
#include "ILightControl.h"

class Light
{
public:
	Light(ILightControl& ILightControl, unsigned int id, unsigned int lampId, const std::wstring& name) : m_ILightControl(ILightControl), m_Id(id), m_LampId(lampId), m_Name(name), m_State(/*ILightControl.ReadLightState(m_LampId)*/) {
	};

	unsigned int GetId() const { return m_Id; };
	const std::wstring& GetName() const { return m_Name; };
	LightState GetState() const { return m_State; };
	void UpdateState(LightState newState) { if (m_ILightControl.SetLightState(m_LampId, newState)) m_State = newState; };
	LightState ReadBackState() { m_State = m_ILightControl.ReadLightState(m_LampId); return m_State; };

private:
	ILightControl& m_ILightControl;
	const unsigned int m_Id;
	const unsigned int m_LampId;
	const std::wstring m_Name;
	LightState m_State;
};


