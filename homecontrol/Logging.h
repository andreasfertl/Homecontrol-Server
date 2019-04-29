#pragma once
#include <sstream>
#include <iostream>
#include <string>
#include <string.h>
#include "IPrint.h"

#define __FILENAME__ (strrchr(__FILE__, '\\') ? strrchr(__FILE__, '\\') + 1 : __FILE__)


#define Logg(_iPrint, _message) {                                                                                           \
							std::wostringstream messageStream, printStream;                                                 \
							messageStream << _message;                                                                      \
							printStream << __FILENAME__ << L" Line:" << __LINE__ << L" " << messageStream.str() << L"\r\n"; \
							_iPrint.Print(printStream.str());                                                               \
					   }

