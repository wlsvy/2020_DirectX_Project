#pragma once
#include <chrono>

struct TimeInfo {
	friend class Timer;
public:
	float GetTime();
	float GetDeltaTime();
private:
	float time = -1.0f;
	float deltaTime;
};

class Timer {
public:
	Timer();
	float GetMilisecondsElapsed();
	void Restart();
	bool Stop();
	bool Start();
	void Tick();

	TimeInfo Time;
private:
	bool isrunning = false;
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