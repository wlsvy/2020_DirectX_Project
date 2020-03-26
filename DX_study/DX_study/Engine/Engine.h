#pragma once
#include "../WindowContainer.h"
#include "../Timer.h"
#include "SceneManager.h"
#include "PhysicsManager.h"
#include "AnimationManager.h"
#include "../Util/Singleton.h"
#include "ScriptBehaviourManager.h"
#include <memory>

class Light_ver2;
class ScriptBehaviour;

class Engine : public WindowContainer, public Singleton<Engine> {
public:
	Engine();

	bool Initialize(HINSTANCE hInstance, std::string window_title, std::string window_class, int width, int height);
	bool ProcessMessage();
	void Update();
	void RenderFrame();

	void RegisterComponent(const std::shared_ptr<ScriptBehaviour>& compo);
	void RegisterComponent(const std::shared_ptr<Light_ver2> & compo);
	void RegisterComponent(const std::shared_ptr<Collider_v2> & compo);
	void RegisterComponent(const std::shared_ptr<Animator> & compo);
	void RegisterComponent(const std::shared_ptr<Terrain> & compo);

	void Component_Valid_Test();

	GraphicsManager& GetGraphicsModule();
	PhysicsModule& GetPhysicsManager();
	SceneManager& GetSceneManager();
	KeyboardClass& GetKeyboard();
	MouseClass& GetMouse();

	std::vector<AnimationClip> * GetAnimClipBuffer();

	~Engine();

private:
	Timer m_Timer;
	GraphicsManager m_GraphicsManager;
	ScriptBehaviourManager m_ScriptBehaviourManager;
	SceneManager m_SceneManager;
	//PhysicsModule m_PhysicsManager;
	AnimationManager m_AnimationManager;

	std::shared_ptr<PhysicsModule>			m_PhysicsManager;

	std::vector<std::shared_ptr<ScriptBehaviour>> scriptBuffer;
	std::vector<std::shared_ptr<Light_ver2>> lightBuffer;
	std::vector<std::shared_ptr<Terrain>> terrainBuffer;
	std::vector<std::shared_ptr<Animator>> animatorBuffer;

	float m_FixedTimeCheck = -1.0f;
	bool ESC_signal = false;
};

