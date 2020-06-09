#include "Timer.h"

using namespace std::chrono;
using Clock = high_resolution_clock;

Timer::Timer() {
	m_StartTime = Clock::now();
}

void Timer::Stop() {
	m_IsRunning = false;
}

void Timer::Start() {
	if (!m_IsRunning) 
	{
		m_StartTime = Clock::now();
		m_PrevTick = Clock::now();
		m_IsRunning = true;
	}
}

void Timer::Tick() {
	if (!m_IsRunning) {
		m_DeltaTime = 0.0f;
		return;
	}

	auto now = Clock::now();
	m_Time = duration<float>(now - m_StartTime).count();
	m_DeltaTime = duration<float>(now - m_PrevTick).count();
	m_PrevTick = now;
}

bool Timer::IsRunning()
{
	return m_IsRunning;
}

float Timer::GetTime()
{
	return m_Time;
}

float Timer::GetDeltaTime()
{
	return m_DeltaTime;
}