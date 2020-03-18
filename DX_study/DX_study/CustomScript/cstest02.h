#pragma once
#include "../Component/ScriptBehaviour.h"

class cstest02 : public ScriptBehaviour {
public:
	NEVER_USE_CONSTRUCTOR(cstest02)

	//cstest cs;
	cstest* tmpcs;
	float test = 1.0f;

	void Start() override;

	void Update() override;
	
};