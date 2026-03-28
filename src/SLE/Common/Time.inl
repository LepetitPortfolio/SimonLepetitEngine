#include "Time.h"

template<typename Rep, typename Period>
inline Time::Time(const std::chrono::duration<Rep, Period>& _Duration) : m_Duration(_Duration)
{
}

template<typename Rep, typename Period>
inline Time::operator std::chrono::duration<Rep, Period>() const
{
	return m_Duration;
}
