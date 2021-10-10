#pragma once
#include <list>
#include "iRESTApi.h"

namespace RESTAPI {
	struct command {
	public:
		command(const utility::string_t& command, const utility::string_t& value, const std::function<std::optional<web::json::value>(const utility::string_t& value)>& callback, const Message& messageToRequest) :
			m_Command(command),
			m_Value(value),
			m_Callback(callback),
			m_MessageToRequest(messageToRequest)
		{
		}
		const utility::string_t m_Command;
		const utility::string_t m_Value;
		const std::function<std::optional<web::json::value>(const utility::string_t& value)> m_Callback;
		const Message m_MessageToRequest;

	};

}

class RESTApi : public iRESTApi {

public:
	RESTApi(struct IJSONManager& iJSONManager);
	~RESTApi();

	RESTApi(const RESTApi&) = default;
	RESTApi& operator = (const RESTApi&) = default;
	RESTApi(RESTApi&&) = default;
	RESTApi& operator = (RESTApi&&) = default;

	//iRESTApi
	std::optional<web::json::value> parseGetReqeust(const std::map<utility::string_t, utility::string_t>& values) const override;
	std::optional<Message> parseGetRequestAndGenerateMessage(const std::map<utility::string_t, utility::string_t>& values) const override;

private:
	struct IJSONManager& m_IJSONManager;
	const std::map<utility::string_t, std::list<RESTAPI::command>> m_Commands;
};

