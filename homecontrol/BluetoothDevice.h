#pragma once
#include <string>

class BluetoothDevice
{

public:
	BluetoothDevice() :
		m_DeviceId(0),
		m_Address(),
		m_InRange(false)
	{
	}

	BluetoothDevice(int deviceId, const std::wstring& addr, bool inRange) :
		m_DeviceId(deviceId),
		m_Address(addr),
		m_InRange(inRange)
	{
	}

	bool operator==(const BluetoothDevice& rhs) const {
		return m_Address == rhs.m_Address && m_InRange == rhs.m_InRange && m_DeviceId == rhs.m_DeviceId;
	}

	int GetDeviceId() const { return m_DeviceId; }
	const std::wstring& GetAddress() const { return m_Address; }
	bool GetInRange() const { return m_InRange; }
	std::wstring ToString() {
		return L"BtDevice id: " + std::to_wstring(m_DeviceId) + L" Addr: " + m_Address + L" Range " + std::to_wstring(m_InRange);
	}

private:
	int          m_DeviceId;
	std::wstring m_Address;
	bool         m_InRange;

};
