#pragma once

enum class Id {
	Run,
	Temperature,
	Humidity,
	LightState = 5,
	BlootoothDevice = 6,
	Alive = 15,
	ArctechSwitch = 16,
	MandolynSensor = 17,
	TelldusDeviceSetState = 18,
	SoundPlay,
	SoundPause,
	Subscribe = 21,
};

enum class Cmd {
	Write = 0,
	Read = 1,
	Answer = 2,
	Any = 3,
	Run = 6,
};
