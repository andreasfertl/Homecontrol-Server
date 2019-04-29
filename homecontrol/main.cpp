#include "PrintQueue.h"
#include "ProgramManager.h"

#ifdef _WIN32

#define WIN32_LEAN_AND_MEAN   
#include <windows.h>

#endif

ProgramManager* g_ProgramManager = nullptr;

#ifdef _WIN32
BOOL WINAPI ConsoleHandler(DWORD dwType)
{
	switch (dwType) {
	case CTRL_C_EVENT:
		if (g_ProgramManager)
			g_ProgramManager->Stop();
		break;
	default:
		break;
	}
	return TRUE;
}
#endif

int main()
{
#ifdef _WIN32
	SetConsoleCtrlHandler((PHANDLER_ROUTINE)ConsoleHandler, TRUE);
#endif

	PrintQueue printQueue;

	auto programManager = ProgramManager(printQueue);
	g_ProgramManager = &programManager;
	programManager.Run();

}

