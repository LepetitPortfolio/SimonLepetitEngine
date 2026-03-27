#pragma once
#include "../../PlatformConfig.h"

#if PLATFORM_WINDOWS
typedef CursorWin32_cls CursorPlatform;

#include "Win/CursorWin32_cls.h"

#elif PLATFORM_LINUX

#include"Unix/CursorUnix_cls.h"
typedef CursorUnix_cls CursorPlatform;

#endif

