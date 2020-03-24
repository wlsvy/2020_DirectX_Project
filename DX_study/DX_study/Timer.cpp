#include "Timer.h"

float Timer::s_Time = -1.0f;
float Timer::s_DeltaTime = 0.0f;

Timer::Timer() {
	startTime = std::chrono::high_resolution_clock::now();
	stopTime = std::chrono::high_resolution_clock::now();
}

bool Timer::Stop() {
	if (!m_IsRunning) {
		return false;
	}
	else {
		stopTime = std::chrono::high_resolution_clock::now();
		m_IsRunning = false;
		return true;
	}
}

bool Timer::Start() {
	if (m_IsRunning) {
		return false;
	}
	else {
		startTime = std::chrono::high_resolution_clock::now();
		prevTime = std::chrono::high_resolution_clock::now();
		m_IsRunning = true;
		return true;
	}
}

void Timer::Tick()
{
	if (!m_IsRunning) {
		s_DeltaTime = 0.0f;
		return;
	}

	auto now = std::chrono::high_resolution_clock::now();
	s_Time = std::chrono::duration<float>(now - startTime).count();
	s_DeltaTime = std::chrono::duration<float>(now - prevTime).count();
	prevTime = now;
}

float Timer::GetTime()
{
	return s_Time;
}

float Timer::GetDeltaTime()
{
	return s_DeltaTime;
}