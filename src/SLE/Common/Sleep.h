#pragma once
#include "PlatformConfig.h"
class Time;

void Sleep(Time _Duration);

#if PLATFORM_WINDOWS
#include "Win/SleepWin32.h"
#elif PLATFORM_LINUX
#include "Unix/SleepUnix.h"
#endif