#pragma once
#include "../Behaviour.h"


class PrototypeManager : public Behaviour {
	COMPONENT_CONSTRUCTOR(PrototypeManager, Behaviour)
public:
	void Update() override;
	void Awake() override;

private:
};