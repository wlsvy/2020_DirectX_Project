#include "Timer.h"

Timer::Timer() {
	m_StartTime = std::chrono::high_resolution_clock::now();
}

void Timer::Stop() {
	m_IsRunning = false;
}

void Timer::Start() {
	if (!m_IsRunning) 
	{
		m_StartTime = std::chrono::high_resolution_clock::now();
		m_PrevTick = std::chrono::high_resolution_clock::now();
		m_IsRunning = true;
	}
}

void Timer::Tick() {
	if (!m_IsRunning) {
		m_DeltaTime = 0.0f;
		return;
	}

	auto now = std::chrono::high_resolution_clock::now();
	m_Time = std::chrono::duration<double>(now - m_StartTime).count();
	m_DeltaTime = std::chrono::duration<double>(now - m_PrevTick).count();
	m_PrevTick = now;
}

bool Timer::IsRunning()
{
	return m_IsRunning;
}

double Timer::GetTime()
{
	return m_Time;
}

double Timer::GetDeltaTime()
{
	return m_DeltaTime;
}