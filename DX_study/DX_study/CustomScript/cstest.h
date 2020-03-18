#pragma once
#include "../Component/ScriptBehaviour.h"

class cstest : public ScriptBehaviour{
public:
	NEVER_USE_CONSTRUCTOR(cstest);

	float speed = 1.0f;

	void Update() override;
};