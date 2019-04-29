#include "IPrint.h"
#include "Logging.h"
#include "OAuth2.h"
#include "StringTools.h"
#include <cpprest/http_client.h>
#include <cpprest/http_listener.h>
#include <chrono>

#ifdef _WIN32
	#include <windows.h>
	#include <Shellapi.h>
#endif

using namespace utility;
using namespace web;
using namespace web::http;
using namespace web::http::client;
using namespace web::http::oauth2::experimental;
using namespace web::http::experimental::listener;

namespace {

	void OpenBrowserForAuthentication(const std::string& authenticationUrl)
	{
#ifdef _WIN32
		ShellExecuteA(NULL, "open", authenticationUrl.c_str(), NULL, NULL, SW_SHOWNORMAL);
#else

#endif
	}

}

//
// A simple listener class to capture OAuth 2.0 HTTP redirect to localhost.
// The listener captures redirected URI and obtains the token.
// This type of listener can be implemented in the back-end to capture and store tokens.
//
class OAuth2CodeListener
{
public:
	OAuth2CodeListener(IPrint& iPrint, uri listen_uri, oauth2_config& config) :
		m_IPrint(iPrint),
		m_Listener(http_listener(listen_uri)),
		m_Config(config),
		m_ReceivedToken()
	{
		m_Listener.support([this](http::http_request request) -> void {
			if (request.request_uri().path() == U("/") && request.request_uri().query() != U(""))
			{
				m_Resplock.lock();

				m_Config.token_from_redirected_uri(request.request_uri())
					.then([this, request](pplx::task<void> token_task) -> void {
					try
					{
						token_task.wait();
						m_ReceivedToken = std::chrono::steady_clock::now();
						m_TaskCompleted.set(true);
					}
					catch (const oauth2_exception& e)
					{
						Logg(m_IPrint, L"Error: " << StringTools::AsWstring(e.what()));
						m_TaskCompleted.set(false);
					}
				});

				request.reply(status_codes::OK, U("Ok."));

				m_Resplock.unlock();
			}
			else
			{
				request.reply(status_codes::NotFound, U("Not found."));
			}
		});

		try {
			m_Listener.open().wait();
		}
		catch (...) {

		}
	}

	~OAuth2CodeListener() { m_Listener.close().wait(); }

	auto ListenForCode() { return pplx::create_task(m_TaskCompleted); }

	bool AccessTokenExpired() const
	{
		if (m_Config.token().is_valid_access_token())
		{
			auto expirationTime = m_ReceivedToken + std::chrono::seconds(m_Config.token().expires_in());

			if (expirationTime < std::chrono::steady_clock::now())
				return true;  //not expired yet, can use access token
		}
		return false;
	}

private:
	IPrint& m_IPrint;
	http_listener m_Listener;
	pplx::task_completion_event<bool> m_TaskCompleted;
	oauth2_config& m_Config;
	std::mutex m_Resplock;
	std::chrono::time_point<std::chrono::steady_clock> m_ReceivedToken;
};

class Oauth2Session
{
private:
	auto Authorize()
	{
#ifdef _WIN32
		auto url = StringTools::AsString(m_OAuth2Config.build_authorization_uri(true));
#else 
		auto url = m_OAuth2Config.build_authorization_uri(true);
#endif 
		OpenBrowserForAuthentication(url);
		return m_Listener.ListenForCode();
	}

public:
	Oauth2Session(IPrint& iPrint, const utility::string_t& client_key, const utility::string_t& client_secret, const utility::string_t& auth_endpoint,
		           const utility::string_t& token_endpoint, const utility::string_t& redirect_uri,  const utility::string_t& scope) :
		m_IPrint(iPrint),
		m_OAuth2Config(client_key, client_secret, auth_endpoint, token_endpoint, redirect_uri),
		m_Listener(iPrint, redirect_uri, m_OAuth2Config)
	{
		m_OAuth2Config.set_scope(scope);
	}

	void Authenticate()
	{
		Logg(m_IPrint, "Authenticate");

		if (!m_OAuth2Config.token().is_valid_access_token())
		{
			if (Authorize().get())
			{
				m_HttpConfig.set_oauth2(m_OAuth2Config);
			}
			else
			{
				Logg(m_IPrint, "Authentication failed");
			}
		}
	}

	std::optional<std::wstring> GetAccessToken() 	{
		if (m_OAuth2Config.token().is_valid_access_token())	{
			if (!m_Listener.AccessTokenExpired()) {
#ifdef _WIN32
				auto access_token = conversions::to_utf16string(m_OAuth2Config.token().access_token());
#else 
				auto access_token = StringTools::AsWstring(m_OAuth2Config.token().access_token().c_str());
#endif 
				return access_token;
			}
			else { //refresh token
				m_OAuth2Config.token_from_refresh().wait();
				return GetAccessToken(); //now i have a new token and can access it
			}
		}
		return std::nullopt;
	}

	void SetTokenConfig(const configToken& token)
	{
#ifdef _WIN32
		auto tokenToSet = oauth2_token(conversions::to_string_t(token.access_token));
		tokenToSet.set_refresh_token(conversions::to_string_t(token.refresh_token));
		tokenToSet.set_token_type(conversions::to_string_t(token.token_type));
		tokenToSet.set_scope(conversions::to_string_t(token.scope));
#else
		auto tokenToSet = oauth2_token(StringTools::AsString(token.access_token));
		tokenToSet.set_refresh_token(StringTools::AsString(token.refresh_token));
		tokenToSet.set_token_type(StringTools::AsString(token.token_type));
		tokenToSet.set_scope(StringTools::AsString(token.scope));

#endif

		m_OAuth2Config.set_token(tokenToSet);
		m_HttpConfig.set_oauth2(m_OAuth2Config);
	}


private:
	IPrint& m_IPrint;
	http_client_config m_HttpConfig;
	oauth2_config	   m_OAuth2Config;
	OAuth2CodeListener m_Listener;
};



OAuth2::OAuth2(IPrint& iPrint, const std::wstring& secret, const std::wstring& key, const std::wstring& authUrl, const std::wstring& tokenUrl, const std::wstring& scope,
			   const std::wstring& redirectUrl) :
#ifdef _WIN32
	m_Session(new Oauth2Session(iPrint, utility::string_t(key), utility::string_t(secret), utility::string_t(authUrl), utility::string_t(tokenUrl), utility::string_t(redirectUrl), utility::string_t(scope)))
#else
	m_Session(new Oauth2Session(iPrint, StringTools::AsString(key), StringTools::AsString(secret), StringTools::AsString(authUrl), StringTools::AsString(tokenUrl), StringTools::AsString(redirectUrl), StringTools::AsString(scope)))
#endif
{
}

OAuth2::~OAuth2()
{
	delete m_Session;
}

void OAuth2::Authenticate()
{
	if (m_Session)
		m_Session->Authenticate();
}

std::optional<std::wstring> OAuth2::GetAccessToken() const
{
	if (m_Session)
		return m_Session->GetAccessToken();
	else
		return std::nullopt;
}

void OAuth2::SetTokenConfig(const configToken& token)
{
	if (m_Session)
		return m_Session->SetTokenConfig(token);
}
