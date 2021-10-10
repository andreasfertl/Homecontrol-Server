#pragma once
#include <cpprest/json.h>
#include <optional>
#include <map>
#include "Message.h"

struct iRESTApi {
	virtual std::optional<web::json::value> parseGetReqeust(const std::map<utility::string_t, utility::string_t>& values) const = 0;
	virtual std::optional<Message> parseGetRequestAndGenerateMessage(const std::map<utility::string_t, utility::string_t>& values) const = 0;

};
