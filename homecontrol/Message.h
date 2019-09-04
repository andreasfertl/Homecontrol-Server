#pragma once
#include <functional>
#include <iostream>
#include <any>
#include <optional>
#include "ReadMessageInterface.h"
#include "IRunInterface.h"
#include "IPrint.h"
#include "Logging.h"
#include "StringTools.h"

class Message : public IReadMessage
{
public:
	Message(const std::string& fromJson);
	Message(Cmd cmd, Id id, std::any value, std::function<void(Message)> answer);
	Message(Cmd cmd, Id id, std::any value);
	Message(Cmd cmd, Id id);
	~Message();

	//implementation of IReadMessage
	const Cmd&  GetCmd() const  override;
	const Id&   GetId() const override;
	const std::any& GetValue() const override;
	std::string AsJson() const override;

	template<typename T>
	const std::optional<T> GetValue(IPrint* iPrint = nullptr) const {
		if (m_Value.has_value())
		{
			try {
				return std::any_cast<T>(m_Value);
			}
			catch (std::bad_any_cast &e) {
				if (iPrint)
					Logg((*iPrint), StringTools::AsWstring(e.what()));
			}
		}
		return std::nullopt;
	}

	//if the message requires an active answer we can user this
	void Answer(Message msg) const {
		if (m_Answer) {
			m_Answer(msg);
		}
	}

private:
	Cmd         m_Cmd;
	Id          m_Id;
	std::any    m_Value;
	std::function<void(Message)> m_Answer; //if the message requires an active answer we can user this
};
