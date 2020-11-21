#pragma once
#include <cpprest/http_client.h>
#include "IOauth2.h"

class TeslaConnection 
{
public:
	TeslaConnection(struct IPrint& iPrint, struct IOauth2& iOauth2, const std::wstring& vehicleId, const std::wstring& controlUrl, const std::wstring& getExtensionUrl);

	std::wstring get(const std::wstring& cmd);
	std::wstring post(const std::wstring& cmd);

private:
	std::wstring runReqeust(web::http::method mtd, const std::wstring& cmd);
	struct IPrint& m_IPrint;
	struct IOauth2& m_IOauth2;
	const std::wstring m_VehicleId;
	const std::wstring m_ControlUrl;
	const std::wstring m_GetExtensionUrl;
};