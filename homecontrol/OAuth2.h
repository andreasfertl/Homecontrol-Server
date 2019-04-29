#pragma once 
#include <string>
#include "IOauth2.h"


class OAuth2 : public IOauth2
{
public:
	OAuth2(struct IPrint& iPrint, const std::wstring& secret, const std::wstring& key, const std::wstring& authUrl, const std::wstring& tokenUrl, const std::wstring& scope,
		const std::wstring& redirectUrl);
	~OAuth2();

	//IOauth2
	void Authenticate() override;
	std::optional<std::wstring> GetAccessToken() const override;
	void SetTokenConfig(const configToken& token) override;

private:
	class Oauth2Session* m_Session;
};

