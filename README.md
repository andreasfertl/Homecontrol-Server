# Homecontrol-Server
A Homecontrol Server application which runs without any HW dependencies and uses multithreading and messages to communicate between modules.

For Example:

A Lightswitch will send a message to the appropiate light with on/off.

A Temperature sensor will send a message to all subscribers (e.g. logging module) 

## Prerequisites:
Windows 10, 8.1, 7, Linux

Visual Studio 2017 or newer (on Windows)

gcc (on Linux)

Git

vcpkg

cpprestsdk


# Instructions
### Windows
Clone this git repository and use Visual Studio 2017 or 2019 to compile the software.
Additionally you need to install cprrestsdk:
https://github.com/Microsoft/cpprestsdk

cpprestsdk can be installed easily via vcpkg
https://github.com/Microsoft/vcpkg

Please follow the instructions from Microsoft to install vcpkg.
#### Short install vcpk for Windows

```
C:\>mkdir src
C:\>cd src
C:\src>git clone https://github.com/microsoft/vcpkg
Cloning into 'vcpkg'...
remote: Enumerating objects: 27, done.
remote: Counting objects: 100% (27/27), done.
remote: Compressing objects: 100% (27/27), done.
Rremote: Total 88014 (delta 10), reused 2 (delta 0), pack-reused 87987
Receiving objects: 100% (88014/88014), 26.18 MiB | 17.22 MiB/s, done.
Resolving deltas: 100% (54726/54726), done.
Updating files: 100% (5350/5350), done.
C:\src>.\vcpkg\bootstrap-vcpkg.bat

Building vcpkg.exe ...
```

#### Short install version for Windows
```
.\vcpkg.exe install cpprestsdk cpprestsdk:x64-windows boost boost:x64-windows
```

### Linux
On Ubuntu 18.10 i installed via apt these packages to be able to compile the software

```
Linux:~/$ apt install build-essential libcpprest-dev libasio-dev libboost-coroutine-dev -y
```

In the homecontrol folder you find the Makefile.
```
Linux:~/LinuxServer$ cd homecontrol
Linux:~/LinuxServer/homecontrol$ make
Linux:~/LinuxServer/homecontrol$ ./LinuxServer
```

# Configuration
The configuration of this software is done by a .json file (Configuration.json).

### Example config
See the provided Configuration.json in the homecontrol folder.

#### Explanation:
Since this software communicates via messages between modules every configuration item which ends up as a message (e.g. Lights) need to be configured with a unique id (InternalId).

#### Philips Hue support
In the section PhilipsHue you can list up your lights and you need to configure your philips hue bridge ip and key.

#### Telldus (tellstick) support
With the Homecontrol Client this software can use telldus / tellstick lights and remote controllers / lightswitches.
You need to list up lights in the same way in the Telldus section as for Philips hue lights.

#### Remote Client support (RemoteManager)
In the section RemoteManager all command ids of the messages (defined in MessageMetaData.h) which needs to be forwarded activly to the Remote Client. 

#### Sonos integration
You can configure one Sonos Loudspeaker at the moment which will be possible to be controlled.
This software uses the offical API from Sonos.
To use this you need to register a developer account for Sonos and configure your application in the Sonos API.

```
{
	"Configuration": {
		"PhilipsHue": {
			"Lights": [{
				"InternalId": 1001,
				"id": 1,
				"name": "one"
			}, {
				"InternalId": 1002,
				"id": 2,
				"name": "two"
			}, {
				"InternalId": 1003,
				"id": 3,
				"name": "three"
			}],
			"ip": "127.0.0.1",
			"key": "keyForPhilipsHueAccess"
		},
		"Telldus": {
			"Lights": [{
				"InternalId": 1004,
				"id": 1,
				"name": "four"
			}]
		},
		"RemoteManager": {
			"ForwardIds": [{
				"id": 18
			}]
		},
		"Sonos": {
			"authUrl": "https://api.sonos.com/login/v3/oauth",
			"configToken": {
				"access_token": "access token",
				"refresh_token": "refresh token",
				"scope": "playback-control-all",
				"token_type": "Bearer"
			},
			"controlUrl": "https://api.ws.sonos.com/control/api/v1/groups/RINCON_123123123123123:11/playback/",
			"key": "key",
			"redirectUrl": "http://localhost",
			"scope": "playback-control-all",
			"secret": "secret",
			"tokenUrl": "https://api.sonos.com/login/v3/oauth/access"
		},
		"Tesla": {
			"configToken": {
				"access_token": "AccessToken",
				"refresh_token": "-",
				"scope": "-",
				"token_type": "Bearer"
			},
			"controlUrl": "https://owner-api.teslamotors.com/api/1/vehicles",
			"vehicleId": "vehicleID",
			"retryWakeUps": 3,
			"retrySleepInSeconds": 30,
			"fastReadTimeInSeconds": 60,
			"verySlowReadTimeInSeconds": 1800,
			"maxSlowingRateForVerySlowReads": 3
		},
		"Scheduler": {
			"Schedules": [
				{
					"StartHour": 12,
					"StartMinute": 5,
					"ScheduleAction": 1,
					"InternalIds": [
						{
							"InternalId": 1001
						},
						{
							"InternalId": 1002
						}
					]
				},
				{
					"StartHour": 19,
					"StartMinute": 32,
					"ScheduleAction": 1,
					"InternalIds": [
						{
							"InternalId": 1003
						},
						{
							"InternalId": 1004
						}
					]
				},
				{
					"StartHour": 22,
					"StartMinute": 35,
					"ScheduleAction": 0,
					"InternalIds": [
						{
							"InternalId": 1003
						},
						{
							"InternalId": 1004
						}
					]
				}
			],
			"UTCOffset": -2
		}
	}
}
```

