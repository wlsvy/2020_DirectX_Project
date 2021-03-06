#pragma once
#include <chrono>

class Timer {
	friend class Engine;
public:
	Timer();
	void Stop();
	void Start();
	void Tick();
	bool IsRunning();

	float GetTime();
	float GetDeltaTime();

private:
	bool m_IsRunning = false;
	float  m_Time = 0.0f;
	float  m_DeltaTime = 0.0f;

#ifdef _WIN32
	std::chrono::time_point<std::chrono::steady_clock> m_StartTime;
	std::chrono::time_point<std::chrono::steady_clock> m_PrevTick;


#else //윈도우 환경이 아닐 때, ex 리눅스
	std::chrono::time_point<std::chrono::system_clock> m_StartTime;
	std::chrono::time_point<std::chrono::system_clock> m_PrevTick;
#endif
};