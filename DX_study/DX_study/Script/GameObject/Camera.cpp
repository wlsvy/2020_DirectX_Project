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

void Camera::UpdateViewMatrix() //�� ��� ������Ʈ + �̵� ���� ������Ʈ
{
	//ī�޶� ȸ�� ��� ���
	DirectX::XMMATRIX camRotationMatrix = DirectX::XMMatrixRotationRollPitchYaw(m_Transform->rotation.x * 0.0174533f, m_Transform->rotation.y * 0.0174533f, m_Transform->rotation.z * 0.0174533f);
	//ī�޶� Ÿ�� ���� ��� - ī�޶� ���� ���� , ȸ�� ���
	DirectX::XMVECTOR camTarget = XMVector3TransformCoord(Transform::DEFAULT_FORWARD_VECTOR, camRotationMatrix);
	//ī�޶� ���� ��ġ�� �ٸ� ���� ���� ����
	camTarget += m_Transform->GetPositionVector();
	//ī�޶� ���� ������ ���� ������ ����
	DirectX::XMVECTOR upDir = XMVector3TransformCoord(Transform::DEFAULT_UP_VECTOR, camRotationMatrix);
	//�� ��� ����.
	viewMatrix = DirectX::XMMatrixLookAtLH(m_Transform->GetPositionVector(), camTarget, upDir);
	viewProjectionMatrix = viewMatrix * projectionMatrix;
}
