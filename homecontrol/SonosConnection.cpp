#include <cpprest/http_client.h>
#include "Logging.h"
#include "SonosConnection.h"
#include "IOauth2.h"
#include "StringTools.h"

using namespace utility;
using namespace web;
using namespace web::http;
using namespace web::http::client;

SonosConnection::SonosConnection(struct IPrint& iPrint, struct IOauth2& iOauth2, const std::wstring& controlUrl) :
	m_IPrint(iPrint),
	m_IOauth2(iOauth2),
	m_ControlUrl(controlUrl)
{
}

void SonosConnection::SendPost(const std::wstring& cmd)
{
	http_request msg(methods::POST);
	msg.headers().add(U("Content-Type"), U("application/x-www-form-urlencoded;charset=utf-8"));

	//get authentication information
	if (auto accessToken = m_IOauth2.GetAccessToken())
	{
		//add authentication as header data
		std::wstring authCmd(L"Bearer ");
		authCmd += *accessToken;
#ifdef _WIN32
		msg.headers().add(U("Authorization"), conversions::to_string_t(authCmd));
#else 
		msg.headers().add(U("Authorization"), StringTools::AsString(authCmd));
#endif 

		//send cmd as data
		auto url = m_ControlUrl + cmd;

#ifdef _WIN32
		http_client control(conversions::to_string_t(url));
#else 
		http_client control(StringTools::AsString(url));
#endif 
		auto result = control.request(msg).get();

#ifdef _WIN32
		Logg(m_IPrint, result.extract_string().get());
#else 
		Logg(m_IPrint, StringTools::AsWstring(result.extract_string().get()));
#endif 
	}
}








