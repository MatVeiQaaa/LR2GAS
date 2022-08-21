#include <iostream>
#include <thread>
#include <utility>

#include "GaugeIncrementsAsm.h"
#include "mem.h"
#include "winver.h"
#include "framework.h"

#define DEBUG_CONSOLE_ENABLED

uintptr_t g_moduleBase;

double g_winver = 0;
unsigned int g_win10Offset = 0;


DWORD WINAPI HackThread(HMODULE hModule)
{

#ifdef DEBUG_CONSOLE_ENABLED
	AllocConsole();
	FILE* f = nullptr;
	freopen_s(&f, "CONOUT$", "w", stdout);

#endif
	

	g_winver = getSysOpType();
	//winver = 10;
	if (g_winver >= 10)
	{
		g_win10Offset = 0x10000;
	}
	std::cout << "winver: " << g_winver << std::endl;
	std::cout << "win10Offset: " << g_win10Offset << std::endl;

	GetIncrements::HookIncrements();

#ifdef DEBUG_CONSOLE_ENABLED
	while (true)
	{
	}
	fclose(f);
	FreeConsole();
#endif

#pragma warning(push)
#pragma warning(disable : 6258)
	TerminateThread(GetCurrentThread(), 0);
#pragma warning(pop)
	return 0;
}

BOOL APIENTRY DllMain(HMODULE hModule,
	DWORD  ul_reason_for_call,
	LPVOID lpReserved
)
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
	{

		auto* hThread = CreateThread(nullptr, 0, (LPTHREAD_START_ROUTINE)HackThread, hModule, 0, nullptr);
		if (hThread == nullptr)
		{
			std::cout << "Couldn't create main thread\n";
			return FALSE;
		}

		CloseHandle(hThread);
	}
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}
	return TRUE;
}
