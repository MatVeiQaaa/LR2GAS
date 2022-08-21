#pragma once

#include "framework.h"

#include <windows.h>
#include <bcrypt.h>

// FIXME: there a missing `wdm.h` header.

double getSysOpType()
{
    double ret = 0.0;
    NTSTATUS(WINAPI * RtlGetVersion)(LPOSVERSIONINFOEXW);
    OSVERSIONINFOEXW osInfo;

    *(FARPROC*)&RtlGetVersion = GetProcAddress(GetModuleHandleA("ntdll"), "RtlGetVersion");

    if (NULL != RtlGetVersion)
    {
        osInfo.dwOSVersionInfoSize = sizeof(osInfo);
        RtlGetVersion(&osInfo);
        ret = (double)osInfo.dwMajorVersion;
    }
    return ret;
}