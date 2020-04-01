#pragma once
#include <memory>
#include <unordered_map>

#include "WindowContainer.h"

class Timer;
class Graphics;
class DeviceResources;
class Object;

class Engine : WindowContainer{
public:
	static Engine& Get();
	Engine();
	~Engine();

	bool Initialize(HINSTANCE hInstance, std::string window_title, std::string window_class, int width, int height);
	bool ProcessMessage();
	void Update();
	void RenderFrame();

	Timer& GetTimer();
private:
	static Engine* s_Ptr;

	std::unique_ptr<DeviceResources> m_DeviceResources;
	std::unique_ptr<Graphics> m_Graphics;
	std::unique_ptr<Timer> m_Timer;

	std::unordered_map<int, std::shared_ptr<Object>> m_ObjectPool;
};

