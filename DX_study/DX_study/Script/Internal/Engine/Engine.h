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

	Timer& GetTimer();
private:
	static Engine* s_Ptr;

	std::unique_ptr<DeviceResources> m_DeviceResources;
	std::unique_ptr<Graphics> m_Graphics;
	std::unique_ptr<Timer> m_Timer;

	std::unique_ptr<ObjectPool<Object>> m_ObjectPool;
	std::unique_ptr<ObjectPool<GameObject>> m_GameObjPool;
	std::unique_ptr<ObjectPool<Shader>> m_ShaderPool;
	std::unique_ptr<ObjectPool<Texture>> m_TexturePool;
	std::unique_ptr<ObjectPool<Model>> m_ModelPool;

};

