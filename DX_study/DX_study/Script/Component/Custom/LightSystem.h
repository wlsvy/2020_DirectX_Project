#pragma once
#include "../Behaviour.h"

class LightSystem : public Behaviour {
	COMPONENT_CONSTRUCTOR(LightSystem, Behaviour)
public:
	void OnGui(const char* option = "-1") override;
private:
};