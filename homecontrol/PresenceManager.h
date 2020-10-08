#pragma once 

#include <string>
#include <map>
#include "IRuntime.h"
#include "BluetoothDevice.h"

class PresenceManager : public IRuntime
{
public:
	PresenceManager(struct IPrint& iPrint, struct IRuntimeRegister& iRuntimeRegister, struct ISubscribe& iSubscribe);
	~PresenceManager() = default;

	//IRuntime
	void Callback() override;
	void HandleMessage(const Message& msg) override;

private:
	struct IPrint& m_IPrint;
	struct IRuntimeMessageHandling& m_RuntimeMessageHandler;
	std::map<int, BluetoothDevice> m_SomeOneAtHome; //key "device id", value a full bluetooth device (yes, repeats the device id)
	bool m_AtLeastOneAtHome;
};


#pragma once
