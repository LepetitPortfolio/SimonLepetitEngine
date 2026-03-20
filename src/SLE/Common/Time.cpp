#include "Time.h"
#include"../Platform.h"

#include <time.h>
#include <ratio>

#include <cassert>

inline constexpr Time Time::m_ZeroTime;

template<typename Rep, typename Period>
inline Time::Time(const std::chrono::duration<Rep, Period>& _Duration) : m_Duration(_Duration)
{
}

float Time::GetTimeInSeconds() const
{
	return std::chrono::duration<float>(m_Duration).count();
}

std::int32_t Time::GetTimeInMilliseconds() const
{
	return  std::chrono::duration_cast<std::chrono::duration<std::int32_t, std::milli>>(m_Duration).count();
}

std::int64_t Time::GetTimeInMicroseconds() const
{
	return m_Duration.count();
}

std::chrono::microseconds Time::GetDuration() const
{
	return m_Duration;
}

template<typename Rep, typename Period>
inline Time::operator std::chrono::duration<Rep, Period>() const
{
	return m_Duration;
}

void Sleep(Time _Duration)
{
	if (_Duration >= Time::m_ZeroTime)
	{

#if PLATFORM_WINDOWS
		static const UINT periodMin = []
		{
			TIMECAPS tc;
			timeGetDevCaps(&tc, sizeof(TIMECAPS));
			return tc.wPeriodMin;
		}();

		timeBeginPeriod(periodMin);

		::Sleep(static_cast<DWORD>(_Duration.GetTimeInMilliseconds()));

		timeEndPeriod(periodMin);

#elif PLATFORM_LINUX
		const std::int64_t usecs = _Duration.GetTimeInMicroseconds();

		timespec ti{};
		ti.tv_sec = static_cast<time_t>(usecs / 1000000);
		ti.tv_nsec = static_cast<long>((usecs % 1000000) * 1000);

		while ((nanosleep(&ti, &ti) == -1) && (errno == EINTR)) {}
#endif

	}
}

constexpr Time Seconds(float _Amount)
{
	return std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::duration<float>(_Amount));
}

constexpr Time Milliseconds(std::int32_t _Amount)
{
	return std::chrono::milliseconds(_Amount);
}

constexpr Time Microseconds(std::int64_t _Amount)
{
	return std::chrono::microseconds(_Amount);
}

constexpr bool operator==(Time _Left, Time _Right)
{
	return _Left.GetTimeInMicroseconds() == _Right.GetTimeInMicroseconds();
}

constexpr bool operator!=(Time _Left, Time _Right)
{
	return !(_Left == _Right);
}

constexpr bool operator<(Time _Left, Time _Right)
{
	return _Left.GetTimeInMicroseconds() < _Right.GetTimeInMicroseconds();
}

constexpr bool operator<=(Time _Left, Time _Right)
{
	return _Left.GetTimeInMicroseconds() <= _Right.GetTimeInMicroseconds();
}

constexpr bool operator>(Time _Left, Time _Right)
{
	return _Left.GetTimeInMicroseconds() > _Right.GetTimeInMicroseconds();
}

constexpr bool operator>=(Time _Left, Time _Right)
{
	return _Left.GetTimeInMicroseconds() >= _Right.GetTimeInMicroseconds();
}

constexpr Time operator+(Time _Left, Time _Right)
{
	return Microseconds(_Left.GetTimeInMicroseconds() + _Right.GetTimeInMicroseconds());
}

constexpr Time operator-(Time _Left, Time _Right)
{
	return Microseconds(_Left.GetTimeInMicroseconds() - _Right.GetTimeInMicroseconds());
}

constexpr Time& operator+=(Time _Left, Time _Right)
{
	return _Left = _Left + _Right;
}

constexpr Time& operator-=(Time _Left, Time _Right)
{
	return _Left = _Left - _Right;
}

constexpr Time operator*(Time _Left, float _Right)
{
	return Seconds(_Left.GetTimeInSeconds() * _Right);
}

constexpr Time operator*(float _Left, Time _Right)
{
	return Seconds(_Left * _Right.GetTimeInSeconds());
}

constexpr Time operator*(Time _Left, int64_t _Right)
{
	return Microseconds(_Left.GetTimeInMicroseconds() * _Right);
}

constexpr Time operator*(int64_t _Left, Time _Right)
{
	return Microseconds(_Left * _Right.GetTimeInMicroseconds());
}

constexpr Time& operator*=(Time _Left, float _Right)
{
	return _Left = _Left * _Right;
}

constexpr Time& operator*=(Time _Left, int64_t _Right)
{
	return _Left = _Left * _Right;
}

constexpr Time operator/(Time _Left, float _Right)
{
	assert(_Right != 0.0f && "Division by zero is undefined behavior");
	return Seconds(_Left.GetTimeInSeconds() / _Right);
}

constexpr Time operator/(float _Left, Time _Right)
{
	assert(_Right.GetTimeInSeconds() != 0.0f && "Division by zero is undefined behavior");
	return Seconds(_Left / _Right.GetTimeInSeconds());
}

constexpr Time operator/(Time _Left, int64_t _Right)
{
	assert(_Right != 0 && "Division by zero is undefined behavior");
	return Microseconds(_Left.GetTimeInMicroseconds() / _Right);
}

constexpr Time operator/(int64_t _Left, Time _Right)
{
	assert(_Right.GetTimeInMicroseconds() != 0 && "Division by zero is undefined behavior");
	return Microseconds(_Left / _Right.GetTimeInMicroseconds());
}

constexpr Time& operator/=(Time _Left, float _Right)
{
	assert(_Right != 0.0f && "Division by zero is undefined behavior");
	return _Left = _Left / _Right;
}

constexpr Time& operator/=(Time _Left, int64_t _Right)
{
	assert(_Right != 0 && "Division by zero is undefined behavior");
	return _Left = _Left / _Right;
}

constexpr float operator/(Time _Left, Time _Right)
{
	assert(_Right.GetTimeInSeconds() != 0.0f && "Division by zero is undefined behavior");
	return _Left.GetTimeInSeconds() / _Right.GetTimeInSeconds();
}

constexpr Time operator%(Time _Left, Time _Right)
{
	assert(_Right.GetTimeInMicroseconds() != 0 && "Modulo by zero is undefined behavior");
	return Microseconds(_Left.GetTimeInMicroseconds() % _Right.GetTimeInMicroseconds());
}

constexpr Time& operator%=(Time _Left, Time _Right)
{
	assert(_Right.GetTimeInMicroseconds() != 0 && "Modulo by zero is undefined behavior");
	return _Left = _Left % _Right;
}