#include <cstdio>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>
#include <optional>
#include <array>
#include "CommandLineManager.h"
#include "RuntimeId.h"
#include "IRuntimeConfigure.h"
#include "IRuntimeRegister.h"
#include "StringTools.h"

namespace {

	std::string simulatedTdtoolCommand = "Number of devices : 1\r\n"
		"Example device  OFF\r\n"
		"SENSORS :\r\n"
		"PROTOCOL                MODEL                   ID      TEMP    HUMIDITY        RAIN                    WIND                    LAST UPDATED\r\n"
		"mandolyn                temperaturehumidity     11      12.3°   55 % 2020 - 10 - 04 11 : 06 : 02\r\n"
		"fineoffset              temperature             255 - 204.7°                                                                 2020 - 10 - 04 14 : 17 : 53\r\n"
		"mandolyn                temperaturehumidity     12      25.7°   33 % 2020 - 10 - 04 11 : 06 : 08\r\n"
		"mandolyn                temperaturehumidity     21      25.2°   48 % 2020 - 10 - 04 11:06 : 18\r\n"
		"fineoffset              temperature             0 - 204.7°                                                                 2020 - 10 - 03 21 : 28 : 02\r\n"
		"fineoffset              temperaturehumidity     254     0.0°    0 % 2020 - 10 - 04 03 : 48 : 02\r\n"
		"fineoffset              temperature             127 - 201.5°                                                                 2020 - 10 - 04 13 : 14 : 58\r\n"
		"fineoffset              temperature             3 - 204.7°                                                                 2020 - 10 - 03 23 : 01 : 08\r\n"
		"fineoffset              temperaturehumidity     255 - 0.0°   0 % 2020 - 10 - 04 14:04 : 08\r\n"
		"fineoffset              temperature             243 - 204.7°                                                                 2020 - 10 - 04 07:24 : 56\r\n"
		"fineoffset              temperature             253 - 204.7°                                                                 2020 - 10 - 04 11 : 53 : 29\r\n"
		"fineoffset              temperature             61 - 204.7°                                                                 2020 - 10 - 04 10 : 08 : 00\r\n"
		"fineoffset              temperature             223 - 204.7°                                                                 2020 - 10 - 04 11 : 15 : 19\r\n";



	std::optional<std::string> run_CommandLine(const std::string& cmd) {
		std::string result;

#ifdef _WIN32
		result;
#else 
		std::array<char, 128> buffer;
		std::unique_ptr<FILE, decltype(&pclose)> pipe(popen(cmd.c_str(), "r"), pclose);
		if (!pipe) {
			return std::nullopt;
		}
		while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr) {
			result += buffer.data();
		}
#endif 
		return result;
	}

	inline std::optional<std::string> run_CommandLine(std::optional<std::string>& cmd) {
		if (cmd)
			return run_CommandLine(*cmd);
		else
			return std::nullopt;
	}


}


CommandLineManager::CommandLineManager(IPrint& iPrint, IRuntimeRegister& iRuntimeRegister, ISubscribe& iSubscribe) :
	m_IPrint(iPrint),
	m_RuntimeMessageHandler(iRuntimeRegister.RegisterRuntime({ StringTools::AsWstring(__FILE__), runtimeId::CommandLineManager, std::chrono::milliseconds(100) }, * this))
{
	iSubscribe.Subscribe({ Id::CommandLine, m_RuntimeMessageHandler });
}

CommandLineManager::~CommandLineManager()
{
	m_RuntimeMessageHandler.Stop();
}


void CommandLineManager::Callback()
{
}

void CommandLineManager::HandleMessage(const Message& msg)
{
	const auto& cmd = msg.GetCmd();
	if (msg.GetId() == Id::CommandLine) {
		if (cmd != Cmd::Answer) //don't listen to answers
		{
			if (auto commandToRun = msg.GetValue<std::string>(&m_IPrint)) {

				Logg(m_IPrint, L"Running command: " << StringTools::AsWstring(*commandToRun));

				if (cmd == Cmd::Write) {
					run_CommandLine(commandToRun);
				}
				else if (cmd == Cmd::Read) {
					if (auto result = run_CommandLine(commandToRun)) {
						auto lines = StringTools::SplitString(*result, '\n');

						/*for (auto line : lines) {
							Logg(m_IPrint, L"result command: " << StringTools::AsWstring(line));
						}*/

						m_RuntimeMessageHandler.SendMessage(Message(Cmd::Answer, Id::CommandLine, lines));
					}
				}
				else if (cmd == Cmd::ReadWithDirectAnswer) {
					if (auto result = run_CommandLine(commandToRun)) {
						msg.Answer(Message(Cmd::Answer, Id::CommandLine, *result));
					}
				}
			}
		}
	}
}

