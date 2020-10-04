#include <tuple>
#include <regex>
#include "TelldusCommandLineManager.h"
#include "RuntimeId.h"
#include "IRuntimeConfigure.h"
#include "IRuntimeRegister.h"
#include "MandloynSensor.h"


namespace {
	std::string telldusCommand{"tdtool --list"};

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

	
	std::optional<std::tuple<int, double, int>> readSensorData(const std::string& line) {
		//     11      12.3°   55 % 2020 - 10 - 04 11 : 06 : 02\r\n"
		//first one is id (int)
		if (auto identifier = readInt(line)) {
			const auto [id, nextElement] = *identifier;
			if (filterIds(id)) {
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
				return readSensorData(m.suffix().str());
			}
		}
		catch (...) {
			Logg(iPrint, L"Exception happend in iPrint");
			return std::nullopt;
		}
		return std::nullopt;
	}
}


TelldusCommandLineManager::TelldusCommandLineManager(IPrint& iPrint, IRuntimeRegister& iRuntimeRegister, ISubscribe& iSubscribe) :
	m_IPrint(iPrint),
	m_RuntimeMessageHandler(iRuntimeRegister.RegisterRuntime({ StringTools::AsWstring(__FILE__), runtimeId::TelldusCommandLineManager, std::chrono::minutes(5) }, * this))
{
	iSubscribe.Subscribe({ Id::CommandLine, m_RuntimeMessageHandler });
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
	if (msg.GetId() == Id::CommandLine && cmd == Cmd::Answer) {
		if (auto lines = msg.GetValue<std::vector<std::string>>(&m_IPrint)) {
			for (const auto& line : *lines) {
				if (const auto valid = parseLine(line, m_IPrint)) {
					const auto [id, temp, humidity] = *valid;
					m_RuntimeMessageHandler.SendMessage(Message(Cmd::Write, Id::MandolynSensor, MandolynSensor(id, temp, humidity)));
				}
			}
		}
	}
}

