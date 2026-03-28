#pragma once
#include "../PlatformConfig.h"

#if PLATFORM_WINDOWS
#include "Win/CursorWin32_cls.h"
typedef CursorWin32_cls CursorPlatform;

#elif PLATFORM_LINUX

#include"Unix/CursorUnix_cls.h"
typedef CursorUnix_cls CursorPlatform;

#endif

