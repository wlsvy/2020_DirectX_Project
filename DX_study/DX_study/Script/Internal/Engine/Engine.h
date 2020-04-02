#pragma once
#include <memory>
#include <unordered_map>

#include "WindowContainer.h"

class Timer;
class Graphics;
class DeviceResources;

template<typename T>
class ObjectPool;
class Object;
class GameObject;
class Shader;
class Texture;
class Model;

class Engine : WindowContainer{
public:
	static Engine& Get();
	Engine();
	~Engine();

	bool Initialize(HINSTANCE hInstance, std::string window_title, std::string window_class, int width, int height);
	bool ProcessMessage();
	void Update();
	void RenderFrame();

	Graphics& GetGraphics();
	Timer& GetTimer();
private:
	static Engine* s_Ptr;

	std::unique_ptr<Graphics> m_Graphics;
	std::unique_ptr<Timer> m_Timer;

};

