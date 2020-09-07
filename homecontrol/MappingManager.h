#pragma once 

#include <string>
#include <map>
#include "IRuntime.h"
#include "BluetoothDevice.h"

class MappingManager : public IRuntime
{
public:
	MappingManager(struct IPrint& iPrint, struct IRuntimeRegister& iRuntimeRegister, struct ISubscribe& iSubscribe);
	~MappingManager();

	//IRuntime
	void Callback() override;
	void HandleMessage(const Message& msg) override;

private:
	struct IPrint& m_IPrint;
	struct IRuntimeMessageHandling& m_RuntimeMessageHandler;
	std::map<int, BluetoothDevice> m_SomeOneAtHome; //key "device", value in range
	bool m_AtLeastOneAtHome;
};


#pragma once
