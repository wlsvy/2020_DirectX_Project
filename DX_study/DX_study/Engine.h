#pragma once
<<<<<<< HEAD:DX_study/DX_study/Engine/Engine.h
#include <memory>

#include "../WindowContainer.h"
#include "../Util/Singleton.h"

class Timer;
class GraphicsManager;
class ScriptBehaviourManager;
class SceneManager;
class PhysicsModule;
class AnimationManager;
=======
#include "WindowContainer.h"
#include "Timer.h"
#include "SceneManager.h"
#include "PhysicsManager.h"
#include "AnimationManager.h"
#include "ScriptBehaviourManager.h"
#include <memory>
>>>>>>> parent of cb3481a... refactoring:DX_study/DX_study/Engine.h

class Light_ver2;
class ScriptBehaviour;

class Engine : WindowContainer{
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

<<<<<<< HEAD:DX_study/DX_study/Engine/Engine.h
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
=======
	void InsertScriptComponent(ScriptBehaviour * _component, GameObject_v2 *_destination);
	void InsertLightComponent(Light_ver2 * _component, GameObject_v2 *_destination);
	void InsertTerrainComponent(Terrain * _component, GameObject_v2 *_destination);
	void InsertCollider_v2Component(Collider_v2 * _component, GameObject_v2 *_destination);
	void InsertAnimatorComponent(Animator * _component, GameObject_v2 *_destination);
	void Component_Valid_Test();

	SceneManager * getSceneManager();

	std::vector<AnimationClip> * GetAnimClipBuffer();

	inline static Engine& GetInstance() { return *s_Instance; }
	~Engine();

private:
	static Engine* s_Instance;
	Timer timer;
	ScriptBehaviourManager scriptBehaviourManager;
	SceneManager sceneManager;
	PhysicsManager physicsManager;
	AnimationManager animationManager;

	std::vector<std::shared_ptr<ScriptBehaviour>> scriptBuffer;
	std::vector<std::shared_ptr<Light_ver2>> lightBuffer;
	//std::vector<std::shared_ptr<Collider>> physicsBuffer;
	std::vector<std::shared_ptr<Collider_v2>> physicsBuffer2;
	std::vector<std::shared_ptr<Terrain>> terrainBuffer;
	std::vector<std::shared_ptr<Animator>> animatorBuffer;

	float fixedTimeCheck = -1.0f;
	bool ESC_signal = false;
>>>>>>> parent of cb3481a... refactoring:DX_study/DX_study/Engine.h
};

