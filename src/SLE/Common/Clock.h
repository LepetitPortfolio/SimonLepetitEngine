#pragma once

#include <chrono>
#include <ratio>
#include <type_traits>

using ClockType = std::conditional<std::chrono::high_resolution_clock::is_steady, std::chrono::high_resolution_clock, std::chrono::steady_clock>;

static_assert(ClockType::is_steady, "Provided implementation is not a monotonic clock");
static_assert(ClockType::ratio_less_equal_v < ClockType::period, std::micro >, "Clock resolution is too low. Expecting at least a microsecond precision");


class Time;

class Clock
{
public:

	void Start();

	void Stop();

	Time Restart();

	Time Reset();

	bool IsRunning() const;

	Time GetElapsedTime() const;

private:

	ClockType::time_point m_RefPoint{ClockType::now()};
	ClockType::time_point m_StopPoint;

};