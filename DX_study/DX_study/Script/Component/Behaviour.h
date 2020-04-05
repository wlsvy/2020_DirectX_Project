#pragma once
#include "Component.h"

#define BEHAVIOUR_CONSTRUCTOR(type) \
public: \
explicit type(GameObject* gameObj) : Behaviour(gameObj, #type) {} \
explicit type(GameObject * gameObj, const std::string & name) : Behaviour(gameObj, name) {} \

class Behaviour : public Component{
	MANAGED_OBJECT(Behaviour)
	COMPONENT_CONSTRUCTOR(Behaviour)
public:
	virtual ~Behaviour() {}

	virtual void Awake() {}
	virtual void Update() {}

	bool Enable = true;
};