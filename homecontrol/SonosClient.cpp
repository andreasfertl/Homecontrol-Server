#include "SonosClient.h"
#include "Logging.h"
#include "IOauth2.h"
#include "ISonosConnection.h"


SonosClient::SonosClient(struct IPrint& iPrint, struct ISonosConnection& iSonosConnection) :
	m_IPrint(iPrint),
	m_ISonosConnection(iSonosConnection)
{
}


void SonosClient::Pause()
{
	try {
		m_ISonosConnection.SendPost(L"pause");
	}
	catch (...) {
		Logg(m_IPrint, L"exception in SendPost - pause");
	}
}

void SonosClient::Play()
{
	try {
		m_ISonosConnection.SendPost(L"play");
	}
	catch (...) {
		Logg(m_IPrint, L"exception in SendPost - pause");
	}
}










