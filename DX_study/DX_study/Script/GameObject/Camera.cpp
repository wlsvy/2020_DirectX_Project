#include "Camera.h"
#include "../Component/Transform.h"

Camera::Camera() : GameObject("Camera") {
	m_Transform->SetPosition(0.0f, 0.0f, 1.0f);
}

void Camera::SetProjectionValues(float fovDegrees, float aspectRatio, float nearZ, float farZ) {
	float fovRadians = (fovDegrees / 360.0f) * XM_2PI;
	this->projectionMatrix = XMMatrixPerspectiveFovLH(fovRadians, aspectRatio, nearZ, farZ);
}

void Camera::UpdateViewMatrix() //뷰 행렬 업데이트 + 이동 벡터 업데이트
{
	//카메라 회전 행렬 계산
	XMMATRIX camRotationMatrix = XMMatrixRotationRollPitchYaw(m_Transform->rotation.x, m_Transform->rotation.y, m_Transform->rotation.z);
	//카메라 타겟 벡터 계산 - 카메라 정면 벡터 , 회전 행렬
	XMVECTOR camTarget = XMVector3TransformCoord(Transform::DEFAULT_FORWARD_VECTOR, camRotationMatrix);
	//카메라 현재 위치에 다른 시점 방향 조정
	camTarget += m_Transform->GetPositionVector();
	//카메라 현재 각도에 따른 업벡터 조정
	XMVECTOR upDir = XMVector3TransformCoord(Transform::DEFAULT_UP_VECTOR, camRotationMatrix);
	//뷰 행렬 설정.
	viewMatrix = XMMatrixLookAtLH(m_Transform->GetPositionVector(), camTarget, upDir);
	viewProjectionMatrix = viewMatrix * projectionMatrix;
}
