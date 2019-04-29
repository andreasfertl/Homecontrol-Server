#pragma once 
#include "ISonosCMDs.h"

class SonosClient : public ISonosCMDs
{
public:
	SonosClient(struct IPrint& iPrint, struct ISonosConnection& iSonosConnection);

	//ISonosCMDs
	void Pause() override;
	void Play() override;

private:
	struct IPrint& m_IPrint;
	struct ISonosConnection& m_ISonosConnection;

};


