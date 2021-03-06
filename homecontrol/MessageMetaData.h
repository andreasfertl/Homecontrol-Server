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
	Sensor = 22,
	Sensors = 23,
	Lights = 24,
	CommandLine = 25,
	Presence = 26,
	VehicleInformation = 27,
};

enum class Cmd {
	Write = 0,
	Read = 1,
	Answer = 2,
	Any = 3,
	Run = 6,
	ReadWithDirectAnswer = 7,
};
