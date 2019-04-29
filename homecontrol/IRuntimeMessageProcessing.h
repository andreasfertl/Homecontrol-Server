#pragma once


struct IRuntimeMessageProcessing {
	virtual void ProcessMessages(struct ISubscribe& iSubscribe) = 0;
};
