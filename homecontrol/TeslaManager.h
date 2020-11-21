#pragma once
#include <memory>
#include "IRuntime.h"
#include "TeslaConnection.h"
#include "TeslaDataStructs.h"
#include "OneShotTimer.h"

class TeslaAuthStorage : public IOauth2
{
public:
	TeslaAuthStorage(const configToken& token) : m_ConfigToken(token) {};
	~TeslaAuthStorage() {};

	//IOauth2
	void Authenticate() override {};
	std::optional<std::wstring> GetAccessToken() const override {
		return m_ConfigToken.access_token;
	}
	void SetTokenConfig(const configToken& token) override { m_ConfigToken = token; };

private:
	configToken m_ConfigToken;

};

class TeslaManager : public IRuntime
{
	enum class ReaderStates {
		CheckState,
		CarIsAwake,
		CarIsAwakeAndDriving,
		CarIsAwakeAndCharging,
		EnterCarIsSleepingMode,
		CarIsSleeping,
		WaitForStateChange,
	};

public:
	TeslaManager(struct IPrint& iPrint, struct IConfigurationTeslaManager& iGetConfiguration, struct IRuntimeRegister& iRuntimeRegister, struct ISubscribe& iSubscribe);
	~TeslaManager();

	//IRuntime
	void Callback() override;
	void HandleMessage(const Message& msg) override;

private:
	struct IPrint& m_IPrint;
	TeslaAuthStorage m_TeslaAuthStorage;
	const unsigned int m_RetryWakeUps;
	const unsigned int m_RetrySleepInSeconds;
	const unsigned int m_fastReadTimeInSeconds;
	const unsigned int m_verySlowReadTimeInSeconds;
	const unsigned int m_maxSlowingRateForVerySlowReads;
	TeslaConnection m_TeslaDataConnection;
	TeslaDataStructs::vehicleInformation m_VehicleInformation;
	std::atomic<ReaderStates>      m_State;
	std::atomic<unsigned int>      m_Rate; //rate of freshes
	std::unique_ptr<OneShotTimer>  m_OneShotTimer;
	struct IRuntimeMessageHandling& m_RuntimeMessageHandler;
};


