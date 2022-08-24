#include "framework.h"

#include <iostream>
#include <thread>
#include <utility>

#include "GaugeIncrementsAsm.h"
#include "mem.h"

// TODO: This should be a compiler define.
#define DEBUG_CONSOLE_ENABLED

namespace
{
	uintptr_t g_moduleBase = 0;

	DWORD WINAPI HackThread(HMODULE hModule)
	{

#ifdef DEBUG_CONSOLE_ENABLED
		// TODO: RAII!
		BOOL hResult = AllocConsole();
		if (hResult == NULL)
		{
			// TODO: throw an error.
			// https://docs.microsoft.com/en-us/windows/console/allocconsole#return-value
		}
		// TODO: RAII!
		FILE* f = nullptr;
		freopen_s(&f, "CONOUT$", "w", stdout);

#endif

		GetIncrements::HookIncrements();

#ifdef DEBUG_CONSOLE_ENABLED
		// Infinite loop? Please explain why we need this.
		while (true)
		{
		}

		// WARNING: These lines are unreachable.
		fclose(f);
		FreeConsole();
#endif

#pragma warning(push)
#pragma warning(disable : 6258)
		// TODO: Check the result.
		TerminateThread(GetCurrentThread(), 0);
#pragma warning(pop)
		return 0;
	}
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
		// Why not std::thread?
		HANDLE hThread = CreateThread(nullptr, 0, (LPTHREAD_START_ROUTINE)HackThread, hModule, 0, nullptr);
		if (hThread == nullptr)
		{
			std::cout << "Couldn't create main thread\n";
			return FALSE;
		}

		// Why are we immediately closing the handle of the thread? Why are we not waiting for it to finish?
		// Also, if we need this, check the return result: https://docs.microsoft.com/en-us/windows/win32/api/handleapi/nf-handleapi-closehandle#return-value
		CloseHandle(hThread);
	}
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}
	return TRUE;
}
