#pragma once

#include <any>
#include <string>
#include "MessageMetaData.h"

struct IReadMessage {

	virtual const Cmd&       GetCmd()   const = 0;
	virtual const Id&        GetId()    const = 0;
	virtual const std::any&  GetValue() const = 0;
	virtual std::string      AsJson()   const = 0;

};
