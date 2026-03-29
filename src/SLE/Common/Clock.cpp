#include "Clock.h"
#include "Time.h"

void Clock::Start()
{
	if (!IsRunning())
	{
		m_RefPoint += ClockType::now() - m_StopPoint;
		m_StopPoint = {};
	}
}

void Clock::Stop()
{
	if (IsRunning())
	{
		m_StopPoint = ClockType::now();
	}
}

Time Clock::Restart()
{
	const Time outTime = GetElapsedTime();
	m_RefPoint = ClockType::now();
	m_StopPoint = {};

	return outTime;
}

Time Clock::Reset()
{
	const Time outTime = GetElapsedTime();
	m_RefPoint = ClockType::now();
	m_StopPoint = m_RefPoint;

	return outTime;
}

bool Clock::IsRunning() const
{
	return m_StopPoint == ClockType::time_point();
}

Time Clock::GetElapsedTime() const
{
	if (IsRunning())
	{
		return std::chrono::duration_cast<std::chrono::microseconds>(ClockType::now() - m_RefPoint);
	}
	return std::chrono::duration_cast<std::chrono::microseconds>(m_StopPoint - m_RefPoint);
}
