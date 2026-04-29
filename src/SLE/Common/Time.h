#pragma once
#include <chrono>
#include <cstdint>

class Time
{
public:

	Time() = default;

	template<typename Rep, typename Period>
	Time(const std::chrono::duration<Rep, Period>& _Duration);

	float GetTimeInSeconds() const;

	std::int32_t  GetTimeInMilliseconds() const;

	std::int64_t GetTimeInMicroseconds() const;

	std::chrono::microseconds GetDuration() const;

	template<typename Rep, typename Period>
	operator std::chrono::duration<Rep, Period>() const;

	static const Time m_ZeroTime;

private:

	std::chrono::microseconds m_Duration{};
};

Time Seconds(float _Amount);

Time Milliseconds(std::int32_t _Amount);

Time Microseconds(std::int64_t _Amount);

bool operator==(Time _Left, Time _Right);

bool operator!=(Time _Left, Time _Right);

bool operator<(Time _Left, Time _Right);

bool operator<=(Time _Left, Time _Right);

bool operator>(Time _Left, Time _Right);

bool operator>=(Time _Left, Time _Right);

Time operator+(Time _Left, Time _Right);

Time operator-(Time _Left, Time _Right);

Time& operator+=(Time _Left, Time _Right);

Time& operator-=(Time _Left, Time _Right);

Time operator*(Time _Left, float _Right);

Time operator*(float _Left, Time _Right);

Time operator*(Time _Left, int64_t _Right);

Time operator*(int64_t _Left, Time _Right);

Time& operator*=(Time _Left, float _Right);

Time& operator*=(Time _Left, int64_t _Right);

Time operator/(Time _Left, float _Right);

Time operator/(float _Left, Time _Right);

Time operator/(Time _Left, int64_t _Right);

Time operator/(int64_t _Left, Time _Right);

Time& operator/=(Time _Left, float _Right);

Time& operator/=(Time _Left, int64_t _Right);

float operator/(Time _Left, Time _Right);

Time operator%(Time _Left, Time _Right);

Time& operator%=(Time _Left, Time _Right);

#include "Time.inl"