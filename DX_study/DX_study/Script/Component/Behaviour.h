#pragma once
#include "Component.h"

#define BEHAVIOUR_CONSTRUCTOR(type) \
public: \
explicit type(GameObject* gameObj) : Component(gameObj, #type) {} \
explicit type(GameObject * gameObj, const std::string & name) : Component(gameObj, name) {} \

class Behaviour : public Component{
	MANAGED_OBJECT(Behaviour)
public:
	virtual void Awake();
	virtual void Update();

	bool Enable = true;
};