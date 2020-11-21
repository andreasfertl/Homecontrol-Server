#include "Logging.h"
#include "TeslaConnection.h"
#include "StringTools.h"

using namespace utility;
using namespace web;
using namespace web::http;
using namespace web::http::client;

namespace {
	
	auto buildUrl(http::method mtd, const std::wstring& vehicleId, const std::wstring& controlUrl, const std::wstring& getExtensionUrl, const std::wstring& cmd) {

		if (mtd == methods::GET) {
			return controlUrl + L"/" + vehicleId + L"/" + getExtensionUrl + L"/" + cmd;
		}
		else if (mtd == methods::POST) {
			return controlUrl + L"/" + vehicleId + L"/" + cmd;

		}
		else {
			return std::wstring();
		}
	}

}
 

TeslaConnection::TeslaConnection(IPrint& iPrint, IOauth2& iOauth2, const std::wstring& vehicleId, const std::wstring& controlUrl, const std::wstring& getExtensionUrl)
	: m_IPrint(iPrint)
    , m_IOauth2(iOauth2)
	, m_VehicleId(vehicleId)
    , m_ControlUrl(controlUrl)
	, m_GetExtensionUrl(getExtensionUrl)
{
}

std::wstring TeslaConnection::runReqeust(web::http::method mtd, const std::wstring& cmd)
{
	try {
		http_request msg(mtd);
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

			auto url = buildUrl(mtd, m_VehicleId, m_ControlUrl, m_GetExtensionUrl, cmd);

#ifdef _WIN32
			http_client control(conversions::to_string_t(url));
#else 
			http_client control(StringTools::AsString(url));
#endif 
			auto result = control.request(msg).get();
			auto stringResult = result.extract_string().get();

#ifdef _WIN32
			return stringResult;
#else 
			return StringTools::AsWstring(stringResult);
#endif 
		}
		else {
			return std::wstring();
		}
	}
	catch (...) {
		Logg(m_IPrint, L"exception!");
		return std::wstring();
	}
}

std::wstring TeslaConnection::get(const std::wstring& cmd)
{
	return runReqeust(methods::GET, cmd);
}

std::wstring TeslaConnection::post(const std::wstring& cmd)
{
	return runReqeust(methods::POST, cmd);
}








