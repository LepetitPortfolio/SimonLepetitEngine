#include "Time.h"
#include "PlatformConfig.h"

#include <time.h>
#include <ratio>

#include <cassert>

inline const Time Time::m_ZeroTime;


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

Time Seconds(float _Amount)
{
	return std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::duration<float>(_Amount));
}

Time Milliseconds(std::int32_t _Amount)
{
	return std::chrono::milliseconds(_Amount);
}

Time Microseconds(std::int64_t _Amount)
{
	return std::chrono::microseconds(_Amount);
}

bool operator==(Time _Left, Time _Right)
{
	return _Left.GetTimeInMicroseconds() == _Right.GetTimeInMicroseconds();
}

bool operator!=(Time _Left, Time _Right)
{
	return !(_Left == _Right);
}

bool operator<(Time _Left, Time _Right)
{
	return _Left.GetTimeInMicroseconds() < _Right.GetTimeInMicroseconds();
}

bool operator<=(Time _Left, Time _Right)
{
	return _Left.GetTimeInMicroseconds() <= _Right.GetTimeInMicroseconds();
}

bool operator>(Time _Left, Time _Right)
{
	return _Left.GetTimeInMicroseconds() > _Right.GetTimeInMicroseconds();
}

bool operator>=(Time _Left, Time _Right)
{
	return _Left.GetTimeInMicroseconds() >= _Right.GetTimeInMicroseconds();
}

Time operator+(Time _Left, Time _Right)
{
	return Microseconds(_Left.GetTimeInMicroseconds() + _Right.GetTimeInMicroseconds());
}

Time operator-(Time _Left, Time _Right)
{
	return Microseconds(_Left.GetTimeInMicroseconds() - _Right.GetTimeInMicroseconds());
}

Time& operator+=(Time _Left, Time _Right)
{
	return _Left = _Left + _Right;
}

Time& operator-=(Time _Left, Time _Right)
{
	return _Left = _Left - _Right;
}

Time operator*(Time _Left, float _Right)
{
	return Seconds(_Left.GetTimeInSeconds() * _Right);
}

Time operator*(float _Left, Time _Right)
{
	return Seconds(_Left * _Right.GetTimeInSeconds());
}

Time operator*(Time _Left, int64_t _Right)
{
	return Microseconds(_Left.GetTimeInMicroseconds() * _Right);
}

Time operator*(int64_t _Left, Time _Right)
{
	return Microseconds(_Left * _Right.GetTimeInMicroseconds());
}

Time& operator*=(Time _Left, float _Right)
{
	return _Left = _Left * _Right;
}

Time& operator*=(Time _Left, int64_t _Right)
{
	return _Left = _Left * _Right;
}

Time operator/(Time _Left, float _Right)
{
	assert(_Right != 0.0f && "Division by zero is undefined behavior");
	return Seconds(_Left.GetTimeInSeconds() / _Right);
}

Time operator/(float _Left, Time _Right)
{
	assert(_Right.GetTimeInSeconds() != 0.0f && "Division by zero is undefined behavior");
	return Seconds(_Left / _Right.GetTimeInSeconds());
}

Time operator/(Time _Left, int64_t _Right)
{
	assert(_Right != 0 && "Division by zero is undefined behavior");
	return Microseconds(_Left.GetTimeInMicroseconds() / _Right);
}

Time operator/(int64_t _Left, Time _Right)
{
	assert(_Right.GetTimeInMicroseconds() != 0 && "Division by zero is undefined behavior");
	return Microseconds(_Left / _Right.GetTimeInMicroseconds());
}

Time& operator/=(Time _Left, float _Right)
{
	assert(_Right != 0.0f && "Division by zero is undefined behavior");
	return _Left = _Left / _Right;
}

Time& operator/=(Time _Left, int64_t _Right)
{
	assert(_Right != 0 && "Division by zero is undefined behavior");
	return _Left = _Left / _Right;
}

float operator/(Time _Left, Time _Right)
{
	assert(_Right.GetTimeInSeconds() != 0.0f && "Division by zero is undefined behavior");
	return _Left.GetTimeInSeconds() / _Right.GetTimeInSeconds();
}

Time operator%(Time _Left, Time _Right)
{
	assert(_Right.GetTimeInMicroseconds() != 0 && "Modulo by zero is undefined behavior");
	return Microseconds(_Left.GetTimeInMicroseconds() % _Right.GetTimeInMicroseconds());
}

Time& operator%=(Time _Left, Time _Right)
{
	assert(_Right.GetTimeInMicroseconds() != 0 && "Modulo by zero is undefined behavior");
	return _Left = _Left % _Right;
}