#pragma once

#include <vector>
#include <memory>

class Engine; //include ���� ����
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
	void Script_INIT(ScriptBehaviour * _script);

private:
	ScriptBehaviourManager(
		std::vector<std::shared_ptr<ScriptBehaviour>> * const _buffer, 
		KeyboardClass * const _keyboard, 
		MouseClass * const _mouse,
		PhysicsModule * const _physicsManager,
		KeyboardEvent * const _keyboardEvent,
		MouseEvent * const _mouseEvent)
		:
		mScriptBuffer(_buffer),
		keyboard(_keyboard),
		mouse(_mouse),
		mPhysicsManager(_physicsManager),
		mKeyboardEvent(_keyboardEvent),
		mMouseEvent(_mouseEvent) {}

	void Update();
	void Start();

	std::vector<std::shared_ptr<ScriptBehaviour>> * const mScriptBuffer;
	PhysicsModule * const mPhysicsManager;
	KeyboardClass * const keyboard;
	MouseClass * const mouse;
	KeyboardEvent * const mKeyboardEvent;
	MouseEvent * const mMouseEvent;
};