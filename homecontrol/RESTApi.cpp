#include "RESTApi.h"
#include <array>
#include <optional>
#include <functional>
#include "IJSONManager.h"


namespace {

	void addCommand(std::map<utility::string_t, std::list<RESTAPI::command>>& commandList, const utility::string_t& command, const utility::string_t& value, const std::function<std::optional<web::json::value>(const utility::string_t& value)>& callback) {
		auto found = commandList.find(command);
		if (found != commandList.end()) { //it exists
			auto& list = found->second;
			list.emplace_back(command, value, callback);
		}
		else { //doen't exist
			auto [iterator, ok] = commandList.emplace(std::make_pair(command, std::list<RESTAPI::command>()));
			auto& list = iterator->second;
			list.emplace_back(command, value, callback);
		}
	}

	std::map<utility::string_t, std::list<RESTAPI::command>> generateCommandList(IJSONManager& iJSONManager) {
		std::map<utility::string_t, std::list<RESTAPI::command>> commandList;

		addCommand(commandList, U("sensor"), U("all"), [&iJSONManager](const utility::string_t& value) { return iJSONManager.getAllSensors(); });
		addCommand(commandList, U("sensor"), U("*"), [&iJSONManager](const utility::string_t& value) { 	return iJSONManager.getSensor(stoi(value)); });
		addCommand(commandList, U("light"), U("all"), [&iJSONManager](const utility::string_t& value) { return iJSONManager.getAllLights(); });
		addCommand(commandList, U("light"), U("*"), [&iJSONManager](const utility::string_t& value) { return iJSONManager.getLightState(stoi(value)); });
		addCommand(commandList, U("setLightOn"), U("*"), [&iJSONManager](const utility::string_t& value) { return iJSONManager.setLightState(stoi(value), true); });
		addCommand(commandList, U("setLightOff"), U("*"), [&iJSONManager](const utility::string_t& value) { return iJSONManager.setLightState(stoi(value), false); });
		addCommand(commandList, U("setBTDevPresent"), U("*"), [&iJSONManager](const utility::string_t& value) { return iJSONManager.setBTDevPresent(stoi(value), true); });
		addCommand(commandList, U("setBTDevNotPresent"), U("*"), [&iJSONManager](const utility::string_t& value) { return iJSONManager.setBTDevPresent(stoi(value), false); });
		addCommand(commandList, U("presence"), U("*"), [&iJSONManager](const utility::string_t& value) { return iJSONManager.getPresence(); });

		return commandList;
	}
}

RESTApi::RESTApi(IJSONManager& iJSONManager) :
	m_IJSONManager(iJSONManager),
	m_Commands(generateCommandList(m_IJSONManager))
{
}

RESTApi::~RESTApi() {
}

std::optional<web::json::value> RESTApi::parseGetReqeust(const std::map<utility::string_t, utility::string_t>& values) const {
	//go through alle requested values and handle the first one
	for (auto& elment : values) {
		//found a valid command?
		auto [command, value] = elment;
		auto found = m_Commands.find(command);
		if (found != std::end(m_Commands)) { //found it!
			auto& listOfCommands = found->second;
			for (auto& element : listOfCommands) {
				if (element.m_Value == value) { //thats the one!
					//call function!
					return element.m_Callback(value);
				}
				else if (element.m_Value == U("*")) {
					return element.m_Callback(value);
				}
			}
		}
	}
	return std::nullopt;
}
