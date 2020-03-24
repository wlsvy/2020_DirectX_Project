#include "ModuleResource.h"
#include "Engine.h"
#include "../Graphics/Graphics.h"
#include "../Component/ComponentTypeTraits.h"
#include "ScriptBehaviourManager.h"
#include "PhysicsManager.h"
#include "SceneManager.h"
#include "AnimationManager.h"

Engine & Module::GetEngine()
{
	return Engine::GetInstance();
}

GraphicsManager & Module::GetGraphicsModule()
{
	return Engine::GetInstance().GetGraphicsModule();
}

SceneManager & Module::GetSceneManager()
{
	return Engine::GetInstance().GetSceneManager();
}

ScriptBehaviourManager & Module::GetScriptBehaviourManager()
{
	return Engine::GetInstance().GetScriptBehaviourManager();
}

AnimationManager & Module::GetAnimationManager()
{
	return Engine::GetInstance().GetAnimationManager();
}

PhysicsModule & Module::GetPhysicsModule()
{
	return Engine::GetInstance().GetPhysicsManager();
}

Timer & Module::GetTimer()
{
	return Engine::GetInstance().GetTimer();
}

ID3D11Device & Module::GetDevice()
{
	return Engine::GetInstance().GetGraphicsModule().GetDevice();
}

ID3D11DeviceContext & Module::GetDeviceContext()
{
	return Engine::GetInstance().GetGraphicsModule().GetDeviceContext();
}

void Module::DestroyGameObject(GameObject_v2 & obj)
{
	Engine::GetInstance().GetSceneManager().DestoryGameObject(obj);
}

void Module::RegisterAnimClip(const std::shared_ptr<AnimationClip>& clip)
{
	Engine::
		GetInstance().
		GetAnimationManager().
		RegisterAnimClip(
			clip);
}

void Module::RegisterComponent(std::shared_ptr<ScriptBehaviour> compo)
{
	Engine::GetInstance().GetScriptBehaviourManager().RegisterComponent(compo);
}

void Module::RegisterComponent(std::shared_ptr<Light_ver2> compo)
{
	Engine::GetInstance().GetGraphicsModule().RegisterComponent(compo);
}

void Module::RegisterComponent(std::shared_ptr<Collider_v2> compo)
{
	Engine::GetInstance().GetPhysicsManager().RegisterComponent(compo);
}

void Module::RegisterComponent(std::shared_ptr<Animator> compo)
{
	Engine::GetInstance().GetAnimationManager().RegisterComponent(compo);
}

void Module::RegisterComponent(std::shared_ptr<Terrain> compo)
{
	Engine::GetInstance().GetGraphicsModule().RegisterComponent(compo);
}

void Module::DeregisterComponent(std::shared_ptr<ScriptBehaviour> compo)
{
	Engine::GetInstance().GetScriptBehaviourManager().DeregisterComponent(compo);
}

void Module::DeregisterComponent(std::shared_ptr<Light_ver2> compo)
{
	Engine::GetInstance().GetGraphicsModule().DeregisterComponent(compo);
}

void Module::DeregisterComponent(std::shared_ptr<Collider_v2> compo)
{
	Engine::GetInstance().GetPhysicsManager().DeregisterComponent(compo);
}

void Module::DeregisterComponent(std::shared_ptr<Animator> compo)
{
	Engine::GetInstance().GetAnimationManager().DeregisterComponent(compo);
}

void Module::DeregisterComponent(std::shared_ptr<Terrain> compo)
{
	Engine::GetInstance().GetGraphicsModule().DeregisterComponent(compo);
}