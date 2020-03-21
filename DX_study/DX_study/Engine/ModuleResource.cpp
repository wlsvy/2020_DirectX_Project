#include "ModuleResource.h"
#include "Engine.h"
#include "../Graphics/Graphics.h"
#include "../Component/ComponentTypeTraits.h"
#include "ScriptBehaviourManager.h"
#include "PhysicsManager.h"
#include "SceneManager.h"

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

void Module::RegisterComponent(std::shared_ptr<ScriptBehaviour> compo, ScriptComponentTag)
{
	Engine::GetInstance().GetScriptBehaviourManager().RegisterComponent(compo);
}

void Module::RegisterComponent(std::shared_ptr<Light_ver2> compo, LigthComponentTag)
{
	Engine::GetInstance().GetGraphicsModule().RegisterComponent(compo);
}

void Module::RegisterComponent(std::shared_ptr<Collider_v2> compo, PhysicsComponentTag)
{
	Engine::GetInstance().GetPhysicsManager().RegisterComponent(compo);
}

void Module::RegisterComponent(std::shared_ptr<Animator> compo, terrainComponentTag)
{
	Engine::GetInstance().GetAnimationManager().RegisterComponent(compo);
}

void Module::RegisterComponent(std::shared_ptr<Terrain> compo, AnimationComponentTag)
{
	Engine::GetInstance().GetGraphicsModule().RegisterComponent(compo);
}

void Module::RegisterCompo::RegisterComponent(std::shared_ptr<ScriptBehaviour> compo, ScriptComponentTag)
{
}
