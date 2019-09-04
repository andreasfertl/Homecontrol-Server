#pragma once
#include <string>
#include <cpprest/json.h>

struct IJSONManager {
	virtual web::json::value getSensor(int id) const = 0;
};
