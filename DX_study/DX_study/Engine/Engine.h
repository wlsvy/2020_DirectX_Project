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

	void InsertScriptComponent(ScriptBehaviour * _component, GameObject_v2 *_destination);
	void InsertLightComponent(Light_ver2 * _component, GameObject_v2 *_destination);
	void InsertTerrainComponent(Terrain * _component, GameObject_v2 *_destination);
	void InsertCollider_v2Component(Collider_v2 * _component, GameObject_v2 *_destination);
	void InsertAnimatorComponent(Animator * _component, GameObject_v2 *_destination);
	void Component_Valid_Test();

	SceneManager * getSceneManager();

	std::vector<AnimationClip> * GetAnimClipBuffer();

	~Engine();

private:
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
};

