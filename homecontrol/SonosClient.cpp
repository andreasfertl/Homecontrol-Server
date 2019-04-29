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
	m_ISonosConnection.SendPost(L"pause");
}

void SonosClient::Play()
{
	m_ISonosConnection.SendPost(L"play");
}










