#pragma once 
#include "ISonosConnection.h"

class SonosConnection : public ISonosConnection
{
public:
	SonosConnection(struct IPrint& iPrint, struct IOauth2& iOauth2, const std::wstring& controlUrl);

	//ISonosConnection
	void SendPost(const std::wstring& cmd) override;

private:
	struct IPrint& m_IPrint;
	struct IOauth2& m_IOauth2;
	std::wstring m_ControlUrl;
};


