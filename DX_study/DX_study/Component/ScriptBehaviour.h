#pragma once
#include "Behaviour.h"
#include "../Keyboard/KeyboardClass.h"
#include "../Mouse/MouseClass.h"

class PhysicsModule;

class ScriptBehaviour : public Behaviour {
	friend class ScriptBehaviourManager;
public:
	using ComponentTag = ScriptComponentTag;

	ScriptBehaviour(GameObject_v2 & obj);

	virtual void Start() override;
	virtual void Update() override;
	virtual void FixedUpdate() override;
	//void StartCoroutine();

private:
};

#include "../CustomScript/CustomScriptInclude.h"