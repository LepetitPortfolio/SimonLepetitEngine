#pragma once
#include "../PlatformConfig.h"

#if PLATFORM_WINDOWS

    #ifndef NOMINMAX
    #define NOMINMAX
    #endif

    #ifndef WIN32_LEAN_AND_MEAN
    #define WIN32_LEAN_AND_MEAN
    #endif

    #ifndef _WIN32_WINDOWS
    #define _WIN32_WINDOWS 0x0601
    #endif

    #ifndef _WIN32_WINNT
    #define _WIN32_WINNT 0x0601
    #endif

    #ifndef WINVER
    #define WINVER 0x0601
    #endif

    #ifndef UNICODE
    #define UNICODE 1
    #endif

    #ifndef _UNICODE
    #define _UNICODE 1
    #endif

    #include <windows.h>

#endif

