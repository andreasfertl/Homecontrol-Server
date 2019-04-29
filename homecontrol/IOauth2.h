#pragma once
#include <optional>
#include <string>

struct configToken {
	std::wstring  access_token;
	std::wstring  refresh_token;
	std::wstring  token_type;
	std::wstring  scope;
};

struct IOauth2 {
	virtual void Authenticate() = 0;
	virtual std::optional<std::wstring> GetAccessToken() const = 0;
	virtual void SetTokenConfig(const configToken& token) = 0;
};