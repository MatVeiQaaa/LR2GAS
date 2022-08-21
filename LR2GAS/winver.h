#pragma once

#include "framework.h"

#include <windows.h>
#include <bcrypt.h>

// FIXME: there's a missing `wdm.h` header.

// Not thread-safe.
inline double getSysOpType()
{
    static bool cached = false;
    static double ret = 0.0;

    if (!cached)
    {
        using RtlGetVersionFP = NTSTATUS(WINAPI*)(LPOSVERSIONINFOEXW);

        RtlGetVersionFP RtlGetVersion = nullptr;
        HMODULE ntdllHandle = GetModuleHandleA("ntdll");

        if (ntdllHandle == NULL)
        {
            // TODO: Throw an error.
            // https://docs.microsoft.com/en-us/windows/win32/api/libloaderapi/nf-libloaderapi-getmodulehandlea#return-value
        }

        RtlGetVersion = reinterpret_cast<RtlGetVersionFP>(GetProcAddress(ntdllHandle, "RtlGetVersion"));

        if (RtlGetVersion == NULL)
        {
            // TODO: Throw an error.
            // https://docs.microsoft.com/en-us/windows/win32/api/libloaderapi/nf-libloaderapi-getprocaddress#return-value
        }

        OSVERSIONINFOEXW osInfo = {};
        osInfo.dwOSVersionInfoSize = sizeof(osInfo);
        
        RtlGetVersion(&osInfo);

        ret = static_cast<double>(osInfo.dwMajorVersion);
        cached = true;
    }
    
    return ret;
}
