#pragma once
#include "Component.h"

class Behaviour : public Component{
	MANAGED_OBJECT(Behaviour)
	COMPONENT_CONSTRUCTOR(Behaviour, Component)
public:
	virtual ~Behaviour() {}

	virtual void Awake() {}
	virtual void Update() {}

	bool Enable = true;
};