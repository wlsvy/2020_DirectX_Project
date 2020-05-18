#pragma once
#include "../Behaviour.h"
#include <DirectXMath.h>

class Transform;

class AutoRotation : public Behaviour {
	COMPONENT_CONSTRUCTOR(AutoRotation, Behaviour)
public:
	void Update() override;

private:
	float m_Speed = 6.0f;
	float m_RotateRadius = 30.0f;
	DirectX::XMFLOAT3 m_Center;
	std::shared_ptr<Transform> m_LookAtTarget;
};