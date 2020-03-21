#pragma once

#include <vector>
#include <memory>

class Engine; //include 정의 조심
class PhysicsModule;
class GameObject_v2;
class Component;
class ScriptBehaviour;
class KeyboardClass;
class MouseClass;
class KeyboardEvent;
class MouseEvent;

class ScriptBehaviourManager {
	friend class Engine;
public:
	void RegisterComponent(const std::shared_ptr<ScriptBehaviour>& compo);
	void DeregisterComponent(const std::shared_ptr<ScriptBehaviour>& compo);

private:
	ScriptBehaviourManager() {}

	void Update();
	void Start();

	std::vector<std::shared_ptr<ScriptBehaviour>> m_ScriptBuffer;
};