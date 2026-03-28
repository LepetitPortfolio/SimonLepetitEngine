#include "Sleep.h"

#include "Time.h"

void Sleep(Time _Duration)
{
    if(_Duration >= Time::m_ZeroTime)
    {
        SleepOS(_Duration);
    }
}