#pragma once
#include "iNetworkReceive.h"

class netWorkReceiveHandler : public iNetworkReceive {
public:
	netWorkReceiveHandler(struct IPrint& logger);
	void receive(const networkMessage& nwMessage) const override;

private:
	struct IPrint& m_LoggManager;
};
