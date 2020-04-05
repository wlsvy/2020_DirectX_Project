#pragma once
#include "../Behaviour.h"

class CamMove : public Behaviour {
public:
	void Update() override;

private:
	float m_MoveSpeed = 6.0f;
	float m_FastMoveSpeed = 30.0f;
	float m_RotateSpeed = 0.3f;
};