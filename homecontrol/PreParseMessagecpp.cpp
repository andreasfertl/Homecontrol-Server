#pragma once
#include <any>
#include <functional>
#include "Message.h"
#include "PreparseMessage.h"

namespace PreParseMsg {

	preParseMsg PreParseMsg(class Message& msg)
	{
		auto cmd = msg.GetCmd();

		if (cmd == Cmd::Run)
		{
			auto& runCmd = msg.GetValue();
			if (runCmd.has_value())
			{
				try
				{
					std::any_cast<std::function<void()>>(runCmd)();
					return preParseMsg::handeld;
				}
				catch (std::bad_any_cast &e)
				{
					auto expl = e.what();
					return preParseMsg::error;
				}
			}
			else
			{
				return preParseMsg::error;
			}

		}
		return preParseMsg::unhandeld;
	}

}
