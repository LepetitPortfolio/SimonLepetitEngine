#pragma once
#if defined(_WIN32) || defined(_WIN64)
#include <windows.h>
#define PLATFORM_WINDOWS 1

#elif defined(__linux)
#include <xcb/xcb.h>
#define PLATFORM_LINUX 1

#else 

#error "Unsupported platform"

#endif