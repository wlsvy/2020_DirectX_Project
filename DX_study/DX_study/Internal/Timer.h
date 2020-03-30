#pragma once
#include <chrono>

class Timer {
	friend class Engine;
public:
	double GetTime();
	double GetDeltaTime();

private:
	Timer();
	bool Stop();
	bool Start();
	void Tick();

	bool m_IsRunning = false;
	double  m_Time = 0.0f;
	double  m_DeltaTime = 0.0f;

#ifdef _WIN32
	std::chrono::time_point<std::chrono::steady_clock> m_StartTime;
	std::chrono::time_point<std::chrono::steady_clock> m_PrevTick;


#else //������ ȯ���� �ƴ� ��, ex ������
	std::chrono::time_point<std::chrono::system_clock> m_StartTime;
	std::chrono::time_point<std::chrono::system_clock> m_PrevTick;

#endif
};