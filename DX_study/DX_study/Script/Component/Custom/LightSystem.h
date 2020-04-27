#pragma once
#include "../Behaviour.h"

class LightSystem : public Behaviour {
	COMPONENT_CONSTRUCTOR(LightSystem, Behaviour)
public:
	void Update() override;
	//void Awake() override;
	void OnGui() override;
private:
};