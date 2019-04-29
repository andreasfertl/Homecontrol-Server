#pragma once

struct IRuntimeRegister {
public:
	virtual struct IRuntimeMessageHandling& RegisterRuntime(const struct IRuntimeConfigure& IRuntimeConfigure, struct IRuntime& iThread) = 0;
};
