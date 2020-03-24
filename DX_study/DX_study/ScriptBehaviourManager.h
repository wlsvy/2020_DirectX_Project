#pragma once

#include <vector>
#include <memory>

class Engine; //include 정의 조심
class PhysicsModule;
class GameObject_v2;
class Component;
class ScriptBehaviour;
struct TimeInfo;
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
<<<<<<< HEAD:DX_study/DX_study/Engine/ScriptBehaviourManager.h
	ScriptBehaviourManager() {}
=======
	ScriptBehaviourManager(
		Engine * const _engine_ptr, 
		TimeInfo * const _timeInfo, 
		std::vector<std::shared_ptr<ScriptBehaviour>> * const _buffer, 
		KeyboardClass * const _keyboard, 
		MouseClass * const _mouse,
		PhysicsManager * const _physicsManager,
		KeyboardEvent * const _keyboardEvent,
		MouseEvent * const _mouseEvent)
		:
		engine(_engine_ptr),
		Time(_timeInfo),
		mScriptBuffer(_buffer),
		keyboard(_keyboard),
		mouse(_mouse),
		mPhysicsManager(_physicsManager),
		mKeyboardEvent(_keyboardEvent),
		mMouseEvent(_mouseEvent) {}
>>>>>>> parent of cb3481a... refactoring:DX_study/DX_study/ScriptBehaviourManager.h

	void Update();
	void Start();

<<<<<<< HEAD:DX_study/DX_study/Engine/ScriptBehaviourManager.h
	std::vector<std::shared_ptr<ScriptBehaviour>> m_ScriptBuffer;
=======
	std::vector<std::shared_ptr<ScriptBehaviour>> * const mScriptBuffer;
	Engine * const engine;
	PhysicsManager * const mPhysicsManager;
	TimeInfo * const Time;
	KeyboardClass * const keyboard;
	MouseClass * const mouse;
	KeyboardEvent * const mKeyboardEvent;
	MouseEvent * const mMouseEvent;
>>>>>>> parent of cb3481a... refactoring:DX_study/DX_study/ScriptBehaviourManager.h
};