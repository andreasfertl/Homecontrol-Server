#pragma once
#include "IRuntime.h"
#include "IJSONManager.h"

class JSONManager : public IRuntime, public IJSONManager {

public:
	JSONManager(struct IRuntimeRegister& iRuntimeRegister);
	~JSONManager();

	JSONManager(const JSONManager&) = default;
	JSONManager& operator = (const JSONManager&) = default;
	JSONManager(JSONManager&&) = default;
	JSONManager& operator = (JSONManager&&) = default;

	//IRuntime
	void Callback() override;
	void HandleMessage(const Message& msg) override;

	//IJSONManager
	web::json::value getSensor(int id) const override;

private:
	struct IRuntimeMessageHandling& m_RuntimeMessageHandler;
};

