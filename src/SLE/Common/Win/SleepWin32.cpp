#include "../PlatformConfig.h"

#if PLATFORM_WINDOWS
#include "SleepWin32.h"
#include "../Time.h"

#include "HeaderWin32.h"

#include <mmsystem.h>

void SleepOS(Time _Duration)
{
    static const UINT periodMin = []
    {
        TIMECAPS tc;
        timeGetDevCaps(&tc, sizeof(TIMECAPS));
        return tc.wPeriodMin;
    }();

    timeBeginPeriod(periodMin);

    ::Sleep(static_cast<DWORD>(_Duration.GetTimeInMilliseconds()));

    timeEndPeriod(periodMin);
}

#endif