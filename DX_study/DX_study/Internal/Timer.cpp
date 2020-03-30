#include "Timer.h"

Timer::Timer() {
	m_StartTime = std::chrono::high_resolution_clock::now();
}

bool Timer::Stop() {
	if (!m_IsRunning) {
		return false;
	}
	else {
		m_IsRunning = false;
		return true;
	}
}

bool Timer::Start() {
	if (m_IsRunning) {
		return false;
	}
	else {
		m_StartTime = std::chrono::high_resolution_clock::now();
		m_PrevTick = std::chrono::high_resolution_clock::now();
		m_IsRunning = true;
		return true;
	}
}

void Timer::Tick()
{
	if (!m_IsRunning) {
		m_DeltaTime = 0.0f;
		return;
	}

	auto now = std::chrono::high_resolution_clock::now();
	m_Time = std::chrono::duration<double>(now - m_StartTime).count();
	m_DeltaTime = std::chrono::duration<double>(now - m_PrevTick).count();
	m_PrevTick = now;
}

double Timer::GetTime()
{
	return m_Time;
}

double Timer::GetDeltaTime()
{
	return m_DeltaTime;
}