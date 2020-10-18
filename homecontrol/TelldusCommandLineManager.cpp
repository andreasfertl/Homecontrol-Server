#include <tuple>
#include <regex>
#include "TelldusCommandLineManager.h"
#include "RuntimeId.h"
#include "IRuntimeConfigure.h"
#include "IRuntimeRegister.h"
#include "MandloynSensor.h"
#include "IGetConfiguration.h"
#include "MessageLightState.h"


namespace {
	std::string telldusCommand{"tdtool --list"};
	std::string telldusCommandLightOn{ "tdtool --on " };
	std::string telldusCommandLightOff{ "tdtool --off " };

	//doing this just early to avoid high load due to fail readings on tdtool
	bool filterIds(int id) {
		switch (id) {
		case 11:
		case 12:
		case 21:
			return true;
		default:
			return false;
		}
	}

	std::optional<std::pair<int,std::string>> readInt(const std::string& str) {
		std::regex  regexp("[0-9]+");
		std::smatch m;
		std::regex_search(str, m, regexp);

		if (m.size() >= 1) {
			auto element = m[0];
			return std::make_pair(StringTools::StringToNumber<int>(element), m.suffix().str());
		}
		return std::nullopt;
	}


	std::optional<std::pair<double, std::string>> readDouble(const std::string& str) {
		std::regex regexp(R"([0-9]+(\.[0-9]+)?)");
		std::smatch m;
		std::regex_search(str, m, regexp);

		if (m.size() >= 1) {
			auto element = m[0];
			return std::make_pair(StringTools::StringToNumber<double>(element), m.suffix().str());
		}
		return std::nullopt;
	}

	
	std::optional<std::tuple<int, double, int>> readSensorData(const std::string& line, IPrint& iPrint) {
		//     11      12.3°   55 % 2020 - 10 - 04 11 : 06 : 02\r\n"
		//first one is id (int)
		if (auto identifier = readInt(line)) {
			const auto [id, nextElement] = *identifier;
			if (filterIds(id)) {
				Logg(iPrint, L"filterd to: " << StringTools::AsWstring(line));
				//second is temperature (double)
				if (auto temperature = readDouble(nextElement)) {
					const auto [temp, nextElement] = *temperature;
					//third is humidity (int)
					if (auto humidity = readInt(nextElement)) {
						const auto [humi, nextElement] = *humidity;
						return std::make_tuple(id, temp, humi);
					}
				}
			}
		}
		return std::nullopt;
	}

	std::optional<std::tuple<int, double, int>> parseLine(const std::string& line, IPrint& iPrint) {
		std::regex  regexp("temperaturehumidity");
		std::smatch m;
		std::regex_search(line, m, regexp);

		try {
			if (m.size()) {
				Logg(iPrint, L"Trying to read: " << StringTools::AsWstring(line));
				return readSensorData(m.suffix().str(), iPrint);
			}
		}
		catch (...) {
			Logg(iPrint, L"Exception happend in iPrint");
			return std::nullopt;
		}
		return std::nullopt;
	}

	void AddLight(IPrint& iPrint, std::vector<Light>& lights, ILightControl& ILightControl, unsigned int id, unsigned int lampId, const std::wstring& name)
	{
		lights.emplace_back(ILightControl, id, lampId, name);
		Logg(iPrint, L"Name: " << name << L" ID: " << id << L" internal ID: " << lampId);
	}
}




TelldusCommandLineManager::TelldusCommandLineManager(IPrint& iPrint, struct IConfigurationTelldus& iGetConfiguration, IRuntimeRegister& iRuntimeRegister, ISubscribe& iSubscribe) :
	m_IPrint(iPrint),
	m_RuntimeMessageHandler(iRuntimeRegister.RegisterRuntime({ StringTools::AsWstring(__FILE__), runtimeId::TelldusCommandLineManager, std::chrono::minutes(5) }, * this))
{
	iSubscribe.Subscribe({ Id::CommandLine, m_RuntimeMessageHandler });
	iSubscribe.Subscribe({ Id::LightState, m_RuntimeMessageHandler });

	Logg(m_IPrint, L"Adding telldusLights:");
	auto lights = iGetConfiguration.GetConfigurationTelldus();
	for (auto& light : lights.m_Lights)
		AddLight(m_IPrint, m_Lights, *this, light.m_InternalId, light.m_Id, light.m_Name);
}

TelldusCommandLineManager::~TelldusCommandLineManager()
{
	m_RuntimeMessageHandler.Stop();
}

void TelldusCommandLineManager::Callback()
{
	m_RuntimeMessageHandler.SendMessage(Message(Cmd::Read, Id::CommandLine, telldusCommand));
}

void TelldusCommandLineManager::HandleMessage(const Message& msg)
{
	const auto& cmd = msg.GetCmd();
	const auto& id = msg.GetId();

	//Sensore
	if (id == Id::CommandLine && cmd == Cmd::Answer) {
		if (auto lines = msg.GetValue<std::vector<std::string>>(&m_IPrint)) {
			for (const auto& line : *lines) {
				if (const auto valid = parseLine(line, m_IPrint)) {
					const auto [id, temp, humidity] = *valid;
					m_RuntimeMessageHandler.SendMessage(Message(Cmd::Write, Id::MandolynSensor, MandolynSensor(id, temp, humidity)));
				}
			}
		}
	}
	else if (id == Id::LightState) //light
	{
		if (auto lightstate = msg.GetValue<MessageLightState>(&m_IPrint))
		{
			const auto& cmd = msg.GetCmd();
			if (cmd == Cmd::Write)
			{
				for (auto& lamp : m_Lights) {
					if (lightstate->m_Id == lamp.GetId())
						lamp.UpdateState(lightstate->m_On ? LightState::On : LightState::Off);
				};
			}
			else if (cmd == Cmd::Read)
			{
				for (auto& lamp : m_Lights) {
					if (lightstate->m_Id == lamp.GetId())
						m_RuntimeMessageHandler.SendMessage(Message(Cmd::Answer, Id::LightState, MessageLightState(lamp.GetId(), lamp.GetState() == LightState::On ? true : false, L"")));
				};
			}
			else if (cmd == Cmd::ReadWithDirectAnswer) {
				for (auto& lamp : m_Lights) {
					if (lightstate->m_Id == lamp.GetId()) {
						msg.Answer(Message(Cmd::Answer, Id::LightState, MessageLightState(lamp.GetId(), lamp.GetState() == LightState::On ? true : false, L"")));
					}
				};
			}
		}
	}
}

LightState TelldusCommandLineManager::ReadLightState(unsigned int lamp)
{
	return LightState::Off; //simulating just off for the moment
}

bool TelldusCommandLineManager::SetLightState(unsigned int lamp, LightState state)
{
	//	tdtool --on 12
	std::string commandToSend;

	if (state == LightState::On)
		commandToSend = telldusCommandLightOn;
	else
		commandToSend = telldusCommandLightOff;

	commandToSend += std::to_string(lamp);

	m_RuntimeMessageHandler.SendMessage(Message(Cmd::Write, Id::CommandLine, commandToSend));

	return state == LightState::On;
}

