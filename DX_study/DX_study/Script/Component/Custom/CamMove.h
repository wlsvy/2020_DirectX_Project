#pragma once
#include "../Behaviour.h"

class CamMove : public Behaviour {
	BEHAVIOUR_CONSTRUCTOR(CamMove)
public:
	void Update() override;

private:
	float m_MoveSpeed = 6.0f;
	float m_FastMoveSpeed = 30.0f;
	float m_RotateSpeed = 2.0f;
};