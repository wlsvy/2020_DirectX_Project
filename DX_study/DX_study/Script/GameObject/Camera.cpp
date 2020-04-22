#include "Camera.h"
#include "../Component/Transform.h"
#include "../Internal/Graphics/BaseGeometry.h"

using namespace DirectX;

Camera::Camera() : GameObject("Camera") {
	m_Transform->SetPosition(0.0f, 0.0f, 1.0f);
}

void Camera::SetProjectionValues(float fovDegrees, float aspectRatio, float nearZ, float farZ) {
	float fovRadians = (fovDegrees / 360.0f) * DirectX::XM_2PI;
	projectionMatrix = DirectX::XMMatrixPerspectiveFovLH(fovRadians, aspectRatio, nearZ, farZ);
	viewFrustum = DirectX::BoundingFrustum(projectionMatrix);
}

void Camera::UpdateView()
{
	auto forward = m_Transform->GetForwardVector();
	auto position = m_Transform->positionVec;
	auto viewTarget = forward + position;

	viewMatrix = DirectX::XMMatrixLookAtLH(position, viewTarget, m_Transform->GetUpwardVector());
	viewProjectionMatrix = viewMatrix * projectionMatrix;

	viewFrustum.Origin = m_Transform->position;
	DirectX::XMStoreFloat4(&viewFrustum.Orientation, m_Transform->GetQuaternion());
}
