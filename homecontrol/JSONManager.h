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
	web::json::value getSensor(unsigned int internalId) const override;
	web::json::value getAllSensors() const override;
	web::json::value getLightState(unsigned int internalId) const override;
	web::json::value setLightState(unsigned int internalId, bool state) const override;
	web::json::value setBTDevPresent(unsigned int internalId, bool state) const override;
	web::json::value getAllLights() const override;
	web::json::value getPresence() const override;
	web::json::value getVehicleState() const override;


private:
	struct IRuntimeMessageHandling& m_RuntimeMessageHandler;
};

