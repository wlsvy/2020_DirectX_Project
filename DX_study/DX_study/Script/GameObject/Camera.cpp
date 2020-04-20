#include "Camera.h"
#include "../Component/Transform.h"

using namespace DirectX;

Camera::Camera() : GameObject("Camera") {
	m_Transform->SetPosition(0.0f, 0.0f, 1.0f);
}

void Camera::SetProjectionValues(float fovDegrees, float aspectRatio, float nearZ, float farZ) {
	float fovRadians = (fovDegrees / 360.0f) * DirectX::XM_2PI;
	this->projectionMatrix = DirectX::XMMatrixPerspectiveFovLH(fovRadians, aspectRatio, nearZ, farZ);
}

void Camera::UpdateViewMatrix()
{
	auto viewTarget = m_Transform->GetForwardVector() + m_Transform->GetPositionVector();
	viewMatrix = DirectX::XMMatrixLookAtLH(m_Transform->GetPositionVector(), viewTarget, m_Transform->GetUpwardVector());
	viewProjectionMatrix = viewMatrix * projectionMatrix;
}
