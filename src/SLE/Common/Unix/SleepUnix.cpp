#include "../PlatformConfig.h"

#if PLATFORM_LINUX
#include "SleepUnix.h"

#include "../Time.h"

#include <cerrno>
#include <ctime>

void SleepOS(Time _Duration)
{
    const std::int64_t usecs = _Duration.GetTimeInMicroseconds();

    timespec ti{};
    ti.tv_sec = static_cast<time_t>(usecs / 1000000);
    ti.tv_nsec = static_cast<long>((usecs % 1000000) * 1000);

    while ((nanosleep(&ti, &ti) == -1) && (errno == EINTR)) {}
}

#endif