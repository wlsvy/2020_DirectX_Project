#pragma once
#include <memory>

class Engine;
class GraphicsManager;
class PhysicsModule;
class SceneManager;
class ScriptBehaviourManager;
class AnimationManager;
class Timer;
struct ID3D11Device;
struct ID3D11DeviceContext;

class GameObject_v2;
class Component;
class ScriptBehaviour;
class Light_ver2;
class Collider_v2;
class Animator;
class Terrain;

class AnimationClip;

namespace Module {
	Engine&						GetEngine();
	GraphicsManager&			GetGraphicsModule();
	SceneManager&				GetSceneManager();
	ScriptBehaviourManager&		GetScriptBehaviourManager();
	AnimationManager&			GetAnimationManager();
	PhysicsModule&				GetPhysicsModule();
	Timer&						GetTimer();
	ID3D11Device&				GetDevice();
	ID3D11DeviceContext&		GetDeviceContext();

	void DestroyGameObject(GameObject_v2& obj);

	void RegisterAnimClip(const std::shared_ptr<AnimationClip>& clip);

	void RegisterComponent(std::shared_ptr<ScriptBehaviour> compo);
	void RegisterComponent(std::shared_ptr<Light_ver2> compo);
	void RegisterComponent(std::shared_ptr<Collider_v2> compo);
	void RegisterComponent(std::shared_ptr<Animator> compo);
	void RegisterComponent(std::shared_ptr<Terrain> compo);
	void DeregisterComponent(std::shared_ptr<ScriptBehaviour> compo);
	void DeregisterComponent(std::shared_ptr<Light_ver2> compo);
	void DeregisterComponent(std::shared_ptr<Collider_v2> compo);
	void DeregisterComponent(std::shared_ptr<Animator> compo);
	void DeregisterComponent(std::shared_ptr<Terrain> compo);
	
}