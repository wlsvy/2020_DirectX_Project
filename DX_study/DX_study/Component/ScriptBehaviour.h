#pragma once
#include "Behaviour.h"
#include "../Keyboard/KeyboardClass.h"
#include "../Mouse/MouseClass.h"

class PhysicsModule;

class ScriptBehaviour : public Behaviour {
	friend class ScriptBehaviourManager;
public:
	ScriptBehaviour(const COMPONENT_INIT_DESC & desc);

	virtual void Start() override;
	virtual void Update() override;
	virtual void FixedUpdate() override;
	//void StartCoroutine();

protected:
	KeyboardClass * InputKeyboard = nullptr;
	MouseClass * InputMouse = nullptr;
	PhysicsModule * Physics = nullptr;
private:
};

#include "../CustomScript/CustomScriptInclude.h"