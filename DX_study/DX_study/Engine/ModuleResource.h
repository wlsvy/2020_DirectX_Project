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

namespace Module {
	Engine& GetEngine();
	GraphicsManager& GetGraphicsModule();
	SceneManager& GetSceneManager();
	ScriptBehaviourManager& GetScriptBehaviourManager();
	AnimationManager& GetAnimationManager();
	Timer& GetTimer();
	ID3D11Device& GetDevice();
	ID3D11DeviceContext& GetDeviceContext();

	void DestroyGameObject(GameObject_v2& obj);

	
	struct RegisterCompo {
	public:
		template<typename T>
		void operator() (const std::shared_ptr<T>& ptr) {
			RegisterComponent(ptr, T::ComponentTag);
		}
	private:
		void RegisterComponent(std::shared_ptr<ScriptBehaviour> compo, ScriptComponentTag);
		void RegisterComponent(std::shared_ptr<Light_ver2> compo, LigthComponentTag);
		void RegisterComponent(std::shared_ptr<Collider_v2> compo, PhysicsComponentTag);
		void RegisterComponent(std::shared_ptr<Animator> compo, terrainComponentTag);
		void RegisterComponent(std::shared_ptr<Terrain> compo, AnimationComponentTag);
	};
}