#pragma once 

#include <string>
#include <map>
#include "IRuntime.h"

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
	std::map<std::wstring, bool> m_SomeOneAtHome; //key "bt addr", value in range
	bool m_AtLeastOneAtHome;
};


#pragma once
