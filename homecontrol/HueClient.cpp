#include <cpprest/http_client.h>
#include <cpprest/json.h>
#include "StringTools.h"
#include "HueClient.h"


using namespace web;
using namespace web::http;
using namespace web::http::client;

namespace {
	utility::string_t BuildUrl(const utility::string_t& url, const std::wstring& ip, const std::wstring& key, int lamp)
	{
		utility::ostringstream_t str_url;
#ifdef  _WIN32
		auto _ip = utility::conversions::to_string_t(ip);
		auto _key = utility::conversions::to_string_t(key);
#else
		auto _ip = utility::conversions::to_string_t(StringTools::AsString(ip));
		auto _key = utility::conversions::to_string_t(StringTools::AsString(key));
#endif //  _WIN32

		str_url << U("http://") << _ip << U("/api/") << _key << U("/") << url << U("/") << lamp;
		return str_url.str();
	}

}

HueClient::HueClient(const std::wstring& ip, const std::wstring& key)  : m_Ip(ip), m_Key(key) {
}


LightState HueClient::ReadLightState(unsigned int lamp)
{
	LightState lightState(LightState::Off);
	http_client client(BuildUrl(U("lights"), m_Ip, m_Key, lamp));

	client.request(methods::GET).then([](http_response response) -> pplx::task<json::value> {
		if (response.status_code() == status_codes::OK)
		{
			return response.extract_json();
		}
		// Handle error cases, for now return empty json value... 
		return pplx::task_from_result(json::value());
	}).then([this, lamp, &lightState](pplx::task<json::value> previousTask)
	{
		try
		{
			const json::value& light = previousTask.get();
			auto l = light.serialize();
			if (light.has_field(U("state")))
			{
				auto state = light.at(U("state"));
				if (state.has_field(U("on")))
				{
					auto on = state.at(U("on"));
					if (on.is_boolean())
						lightState = on.as_bool() ? LightState::On : LightState::Off;
				}
				auto stateArray = state.serialize();
			}
		}
		catch (.../*const http_exception& e*/)
		{
			// Print error.
			//auto a = e.what();
		}
	}).wait();

	return lightState;
}

bool HueClient::SetLightState(unsigned int lamp, LightState state)
{
	bool success(false);

	try {
		http_client client(BuildUrl(U("lights"), m_Ip, m_Key, lamp));

		//build json data
		web::json::value parameters = web::json::value::object();
		if (state == LightState::On)
			parameters[U("on")] = web::json::value::boolean(true);
		else
			parameters[U("on")] = web::json::value::boolean(false);

		auto request = client.request(methods::PUT, U("state"), parameters).then([](http_response response) -> bool {
			if (response.status_code() == status_codes::OK)
				return true;
			else
				return false;
			});
		
		success = request.get();
	}
	catch (...) {

	}

	return success;
}

