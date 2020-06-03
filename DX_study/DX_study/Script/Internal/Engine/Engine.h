#pragma once
#include <memory>
#include <unordered_map>

#include "WindowContainer.h"

class Timer;
class Graphics;
class DX11Resources;

template<typename T>
class ObjectPool;
class Object;
class GameObject;
class Shader;
class Texture;
class Model;
class Scene;
class RenderInfo;

class Engine : public WindowContainer{
public:
	static Engine& Get();
	Engine();
	~Engine();

	bool Initialize(HINSTANCE hInstance, std::string window_title, std::string window_class, UINT width, UINT height);
	void Run();

	Graphics& GetGraphics() { return *m_Graphics; }
	Timer& GetTimer() { return *m_Timer; }
	Scene& GetCurrentScene() { return *m_CurrentScene; }
	KeyboardClass& GetKeyboard() { return keyboard; }
	MouseClass& GetMouse() { return mouse; }

	static const float s_FixedFrameRate;

private:
	bool ProcessMessage();
	void Update();
	void UpdateInput();
	void UpdateAnimator();
	void UpdateBehaviour();
	void FixedUpdate();
	void RenderFrame();

	static Engine* s_Ptr;

	std::unique_ptr<Graphics> m_Graphics;
	std::unique_ptr<Timer> m_Timer;
	std::unique_ptr<Scene> m_CurrentScene;

};

