# Homecontrol-Server
A Homecontrol Server application which runs without any HW dependencies.

##Prerequisites:
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

### Linux
On Ubuntu 18.10 i installed via apt these packages to be able to compile the software

```
Linux:~/$ apt install build-essential libcpprest-dev libasio-dev -y
```

In the homecontrol folder you find the Makefile.
```
Linux:~/LinuxServer$ cd homecontrol
Linux:~/LinuxServer/homecontrol$ make
Linux:~/LinuxServer/$ ./LinuxServer
```

# Configuration
The configuration of this software is done by a .json file (Configuration.json).

###Example config


