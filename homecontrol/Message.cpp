#include "Message.h"
#include <cpprest/json.h>
#include "StringTools.h"
#include "ArctechSwitch.h"
#include "MandloynSensor.h"
#include "BluetoothDevice.h"
#include "MessageLightState.h"


namespace {
	std::tuple<Cmd, Id, std::any> Init(const utility::string_t& fromJson)
	{
		Cmd cmd(Cmd::Any);
		Id id(Id::Alive);
		std::any value;

		web::json::value msg = web::json::value::parse(fromJson);

		auto jsonCommand = msg[U("Command")];
		if (jsonCommand.is_integer())
			cmd = static_cast<Cmd>(jsonCommand.as_integer());

		auto jsonCommandType = msg[U("CommandType")];
		if (jsonCommandType.is_integer())
			id = static_cast<Id>(jsonCommandType.as_integer());

		//data part
		auto data = msg[U("Value")];
		if (id == Id::ArctechSwitch)
		{
			//std::wstring a = L"{\"$type\":\"Messanger.Msg,Messanger\",\"Command\":0,\"CommandType\":16,\"Destination\":4294967294,\"ObjectType\":\"Messanger.ArctechSwitch,Messanger,Version = 1.0.0.0,Culture = neutral,PublicKeyToken = null\",\"RemoteHandle\":0,\"Source\":0,\"Value\":{\"$type\":\"Messanger.ArctechSwitch,Messanger\",\"GroupId\":0,\"HouseId\":7733322,\"State\":true,\"UnitId\":1}}";
			auto groupId = data[U("GroupId")].as_integer();
			auto houseId = data[U("HouseId")].as_integer();
			auto state = data[U("State")].as_bool();
			auto unitId = data[U("UnitId")].as_integer();

			value = ArctechSwitch(houseId, unitId, groupId, state);
		}
		else if (id == Id::MandolynSensor)
		{
			//{"$type":"Messanger.Msg, Messanger","Command":0,"CommandType":17,"Destination":4294967294,"ObjectType":"Messanger.MandolynSensor, Messanger, Version=1.0.0.0, Culture=neutral, PublicKeyToken=null","RemoteHandle":0,"Source":0,"Value":{"$type":"Messanger.MandolynSensor, Messanger","Humidity":77,"Id":21,"Temp":8.39999961853027}}
			auto id = data[U("Id")].as_integer();
			auto temp = data[U("Temp")].as_double();
			auto humidity = data[U("Humidity")].as_integer();

			value = MandolynSensor(id, temp, humidity);
		}
		else if (id == Id::BlootoothDevice)
		{
			auto inRange = data[U("InRange")].as_bool();
			auto mac = data[U("BTMacAddr")].as_string();
#ifdef _WIN32
			auto macAddr = mac;
#else 
			auto macAddr = StringTools::AsWstring(mac);
#endif 
			value = BluetoothDevice(macAddr, inRange);
		}
		else if (id == Id::Subscribe)
		{
			//id to subscribe to
			value = data.as_integer();
		}
		else if (id == Id::LightState)
		{
			//id to subscribe to
			auto id = data[U("ID")].as_integer();
			auto onOff = data[U("LightState")].as_integer() ? true : false;
			value = MessageLightState(id, onOff, L"");
		}

		return std::make_tuple(cmd, id, value);
	}

}

Message::Message(const std::string& fromJson) :
	m_Cmd(),
	m_Id(),
	m_Value(),
	m_Answer()
{
	auto json = utility::conversions::to_string_t(fromJson);
	auto[cmd, id, value] = Init(json);
	m_Cmd = cmd;
	m_Id= id;
	m_Value = value;
}

Message::Message(Cmd cmd, Id id, std::any value, std::function<void(Message)> answer) :
	m_Cmd(cmd),
	m_Id(id),
	m_Value(value),
	m_Answer(answer) {
}

Message::Message(Cmd cmd, Id id, std::any value) :
	m_Cmd(cmd),
	m_Id(id),
	m_Value(value),
	m_Answer() {
}

Message::Message(Cmd cmd, Id id) :
	m_Cmd(cmd),
	m_Id(id),
	m_Value(),
	m_Answer() {
}

Message::~Message() {
};


const Cmd& Message::GetCmd() const
{
	return m_Cmd;
}

const Id&  Message::GetId() const
{
	return m_Id; 
}

const std::any& Message::GetValue() const
{
	return m_Value;
}

std::string Message::AsJson() const
{
	web::json::value msg;

	msg[U("Command")] = web::json::value::number(static_cast<int>(m_Cmd));
	msg[U("CommandType")] = web::json::value::number(static_cast<int>(m_Id));

	if (m_Id == Id::TelldusDeviceSetState)
	{
		auto val = std::any_cast<MessageLightState>(m_Value);

		msg[U("Value")][U("$type")] = web::json::value::string(U("Messanger.LightMessage, Messanger"));
		msg[U("Value")][U("ID")] = web::json::value::number(val.m_Id);
		msg[U("Value")][U("LightState")] = web::json::value::number(static_cast<int>(val.m_On));

#ifdef _WIN32
		auto serialized = StringTools::AsString(msg.serialize());
#else 
		auto serialized = msg.serialize();
#endif 
		return serialized;
	}
	else if (m_Id == Id::MandolynSensor)
	{
		auto val = std::any_cast<MandolynSensor>(m_Value);

		msg[U("Value")][U("$type")] = web::json::value::string(U("Messanger.MandolynSensor, Messanger"));
		msg[U("Value")][U("Id")] = web::json::value::number(val.GetId());
		msg[U("Value")][U("Temp")] = web::json::value::number(static_cast<float>(val.GetTemp()));
		msg[U("Value")][U("Humidity")] = web::json::value::number(val.GetHumidity());

#ifdef _WIN32
		auto serialized = StringTools::AsString(msg.serialize());
#else 
		auto serialized = msg.serialize();
#endif 
		return serialized;
	}

	return "";
}
