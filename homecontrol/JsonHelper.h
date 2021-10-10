#pragma once

#include <optional>
#include <cpprest/json.h>
#include "StringTools.h"

namespace JsonHelperN {
	void setUInt64(web::json::value& json, const utility::string_t& value, unsigned long long data);
	void setFloat(web::json::value& json, const utility::string_t& value, float data);
	void setDouble(web::json::value& json, const utility::string_t& value, double data);
	void setBool(web::json::value& json, const utility::string_t& value, bool data);
	void setString(web::json::value& json, const utility::string_t& value, const std::wstring& data);
	void setOptionalString(web::json::value& json, const utility::string_t& value, const std::optional<std::wstring>& data);
	void setInteger(web::json::value& json, const utility::string_t& value, int data);
}