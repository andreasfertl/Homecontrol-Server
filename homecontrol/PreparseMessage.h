#pragma once

namespace PreParseMsg {
	enum class preParseMsg {
		error,
		handeld,
		unhandeld
	};

	PreParseMsg::preParseMsg PreParseMsg(class Message& msg);

}
