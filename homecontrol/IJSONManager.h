#pragma once
#include <string>
#include <cpprest/json.h>

struct IJSONManager {
	virtual web::json::value getSensor(unsigned int internalId) const = 0;
	virtual web::json::value getAllSensors() const = 0;
	virtual web::json::value getLightState(unsigned int internalId) const = 0;
	virtual web::json::value setLightState(unsigned int internalId, bool state) const = 0;
	virtual web::json::value getAllLights() const = 0;
	virtual web::json::value getPresence() const = 0;
};
