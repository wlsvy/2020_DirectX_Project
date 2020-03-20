#pragma once
#include <chrono>

class Timer {
	friend class Engine;
public:
	static float GetTime();
	static float GetDeltaTime();

private:
	Timer();
	bool Stop();
	bool Start();
	void Tick();

	bool m_IsRunning = false;
	static float s_Time;
	static float s_DeltaTime;

#ifdef _WIN32
	std::chrono::time_point<std::chrono::steady_clock> startTime;
	std::chrono::time_point<std::chrono::steady_clock> stopTime;
	std::chrono::time_point<std::chrono::steady_clock> prevTime;
	std::chrono::time_point<std::chrono::steady_clock> baseTime;


#else //윈도우 환경이 아닐 때, ex 리눅스
	std::chrono::time_point<std::chrono::system_clock> startTime;
	std::chrono::time_point<std::chrono::system_clock> stopTime;
	std::chrono::time_point<std::chrono::system_clock> prevTime;
	std::chrono::time_point<std::chrono::system_clock> baseTime;

#endif
};