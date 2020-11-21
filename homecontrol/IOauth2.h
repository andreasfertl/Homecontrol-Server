#pragma once
#include <optional>
#include <string>

struct configToken {
	configToken(const std::wstring& access_token, const std::wstring& refresh_token, const std::wstring& token_type, const std::wstring& scope)
		: access_token(access_token)
		, refresh_token(refresh_token)
		, token_type(token_type)
		, scope(scope)
	{
	};

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