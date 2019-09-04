#pragma once
#include <string>

struct IJSONManager {
	virtual void getSensor(int id) const = 0;
};
