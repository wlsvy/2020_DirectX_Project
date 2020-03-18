#include "Timer.h"

Timer::Timer() {
	startTime = std::chrono::high_resolution_clock::now();
	stopTime = std::chrono::high_resolution_clock::now();
}

float Timer::GetMilisecondsElapsed() {
	if (isrunning) {
		auto elapsed = std::chrono::duration<float, std::milli>(std::chrono::high_resolution_clock::now() - startTime);
		return elapsed.count();
	}
	else {
		auto elapsed = std::chrono::duration<float, std::milli>(stopTime - startTime);
		return elapsed.count();
	}
}

void Timer::Restart() {
	isrunning = true;
	startTime = std::chrono::high_resolution_clock::now();
	prevTime = startTime;
}

bool Timer::Stop() {
	if (!isrunning) return false;
	else {
		stopTime = std::chrono::high_resolution_clock::now();
		isrunning = false;
		return true;
	}
}

bool Timer::Start() {
	if (isrunning) return false;
	else {
		startTime = std::chrono::high_resolution_clock::now();
		prevTime = std::chrono::high_resolution_clock::now();
		isrunning = true;
		return true;
	}
}

void Timer::Tick()
{
	if (!isrunning) {
		Time.deltaTime = 0.0f;
		return;
	}

	auto now = std::chrono::high_resolution_clock::now();
	Time.time = std::chrono::duration<float>(now - startTime).count();
	Time.deltaTime = std::chrono::duration<float>(now - prevTime).count();
	prevTime = now;

	//assert("Time.deltaTime have minus value." && Time.deltaTime >= 0.0f);
}

float TimeInfo::GetTime()
{
	return time;
}

float TimeInfo::GetDeltaTime()
{
	return deltaTime;
}
