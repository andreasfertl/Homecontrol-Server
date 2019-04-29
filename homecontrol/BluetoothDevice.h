#pragma once
#include <string>

class BluetoothDevice
{

public:
	BluetoothDevice(const std::wstring& addr, bool inRange) :
		m_Address(addr),
		m_InRange(inRange)
	{
	}

	const std::wstring& GetAddress() { return m_Address; }
	const bool GetInRange() { return m_InRange; }

	bool operator==(const BluetoothDevice& rhs) const {
		return m_Address == rhs.m_Address && m_InRange == rhs.m_InRange;
	}

private:
	const std::wstring m_Address;
	const bool        m_InRange;

};
