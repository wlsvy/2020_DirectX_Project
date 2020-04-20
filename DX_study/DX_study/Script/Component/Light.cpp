#include "Light.h"
#include "Transform.h"
#include "../Internal/Core/GameObject.h"

using DirectX::operator+;

void LightBase::Awake() {

}

const DirectX::XMMATRIX & SpotLight::GetLightViewProjectMat() const
{
	float fovRadians = (SpotAngle / 360.0f) * DirectX::XM_2PI;
	auto projectionMatrix = DirectX::XMMatrixPerspectiveFovLH(fovRadians, 1.0f, 0.1f, Range);

	auto& transform = m_GameObject->GetTransform();

	auto viewTarget = transform.GetForwardVector() + transform.GetPositionVector();
	auto viewMatrix = DirectX::XMMatrixLookAtLH(transform.GetPositionVector(), viewTarget, transform.GetUpwardVector());

	return viewMatrix * projectionMatrix;
}

void SpotLight::OnGui()
{
}