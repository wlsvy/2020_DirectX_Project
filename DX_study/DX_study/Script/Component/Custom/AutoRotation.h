#pragma once
#include "../Behaviour.h"

class AutoRotation : public Behaviour {
	COMPONENT_CONSTRUCTOR(AutoRotation, Behaviour)
public:
	void Update() override;

private:
	float m_MoveSpeed = 6.0f;
	float m_FastMoveSpeed = 30.0f;
	float m_RotateSpeed = 100.0f;
};