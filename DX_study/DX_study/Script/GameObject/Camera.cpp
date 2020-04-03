#include "Camera.h"
#include "../Component/Transform.h"

Camera::Camera() : GameObject("Camera") {
	m_Transform->SetPosition(0.0f, 0.0f, 1.0f);
}

void Camera::SetProjectionValues(float fovDegrees, float aspectRatio, float nearZ, float farZ) {
	float fovRadians = (fovDegrees / 360.0f) * XM_2PI;
	this->projectionMatrix = XMMatrixPerspectiveFovLH(fovRadians, aspectRatio, nearZ, farZ);
}

void Camera::UpdateViewMatrix() //�� ��� ������Ʈ + �̵� ���� ������Ʈ
{
	//ī�޶� ȸ�� ��� ���
	XMMATRIX camRotationMatrix = XMMatrixRotationRollPitchYaw(m_Transform->rotation.x, m_Transform->rotation.y, m_Transform->rotation.z);
	//ī�޶� Ÿ�� ���� ��� - ī�޶� ���� ���� , ȸ�� ���
	XMVECTOR camTarget = XMVector3TransformCoord(Transform::DEFAULT_FORWARD_VECTOR, camRotationMatrix);
	//ī�޶� ���� ��ġ�� �ٸ� ���� ���� ����
	camTarget += m_Transform->GetPositionVector();
	//ī�޶� ���� ������ ���� ������ ����
	XMVECTOR upDir = XMVector3TransformCoord(Transform::DEFAULT_UP_VECTOR, camRotationMatrix);
	//�� ��� ����.
	viewMatrix = XMMatrixLookAtLH(m_Transform->GetPositionVector(), camTarget, upDir);
	viewProjectionMatrix = viewMatrix * projectionMatrix;
}
