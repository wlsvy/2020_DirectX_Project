#pragma once
#include <memory>

#include "WindowContainer.h"

class Timer;
class Graphics;

class Engine : WindowContainer{
public:
	static Engine& GetInstance();
	Engine();
	~Engine();

	bool Initialize(HINSTANCE hInstance, std::string window_title, std::string window_class, int width, int height);
	bool ProcessMessage();
	void Update();
	void RenderFrame();

	Timer& GetTimer();
private:
	static Engine* s_Ptr;

	std::unique_ptr<Graphics> m_Graphics;
	std::unique_ptr<Timer> m_Timer;
};

