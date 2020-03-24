#pragma once
#include <memory>

#include "../WindowContainer.h"
#include "../Util/Singleton.h"

class Timer;
class GraphicsManager;
class ScriptBehaviourManager;
class SceneManager;
class PhysicsModule;
class AnimationManager;

class Light_ver2;
class ScriptBehaviour;

class Engine : public WindowContainer, public Singleton<Engine> {
public:
	Engine();

	bool Initialize(
		HINSTANCE hInstance, 
		std::string window_title, 
		std::string window_class, 
		int width, 
		int height);
	bool ProcessMessage();
	void Update();
	void RenderFrame();

	GraphicsManager&		GetGraphicsModule();
	PhysicsModule&			GetPhysicsManager();
	SceneManager&			GetSceneManager();
	ScriptBehaviourManager& GetScriptBehaviourManager();
	AnimationManager&		GetAnimationManager();
	KeyboardClass&			GetKeyboard();
	MouseClass&				GetMouse();
	Timer&					GetTimer();

private:
	std::shared_ptr<Timer>					m_Timer;
	std::shared_ptr<GraphicsManager>		m_GraphicsManager;
	std::shared_ptr<ScriptBehaviourManager> m_ScriptBehaviourManager;
	std::shared_ptr<SceneManager>			m_SceneManager;
	std::shared_ptr<PhysicsModule>			m_PhysicsManager;
	std::shared_ptr<AnimationManager>		m_AnimationManager;

	float m_FixedTimeCheck = -1.0f;
	bool m_QuitFlag = false;
};

