#pragma once
#include <string>
#include <cpprest/json.h>

struct IJSONManager {
	virtual web::json::value getSensor(unsigned int internalId) const = 0;
};
