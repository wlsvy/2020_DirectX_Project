#include "Camera.h"

Camera3D::Camera3D() {
	this->pos = XMFLOAT3(0.0f, 0.0f, 1.0f);
	this->posVector = XMLoadFloat3(&this->pos);
	this->rot = XMFLOAT3(0.0f, 0.0f, 0.0f);
	this->rotVector = XMLoadFloat3(&this->rot);
	UpdateMatrix();
}

void Camera3D::SetProjectionValues(float fovDegrees, float aspectRatio, float nearZ, float farZ) {
	float fovRadians = (fovDegrees / 360.0f) * XM_2PI;
	this->projectionMatrix = XMMatrixPerspectiveFovLH(fovRadians, aspectRatio, nearZ, farZ);
}

const DirectX::XMMATRIX & Camera3D::GetViewMatrix() const
{
	return this->viewMatrix;
}

const DirectX::XMMATRIX & Camera3D::GetProjectionMatrix() const
{
	return this->projectionMatrix;
}


void Camera3D::UpdateMatrix() //�� ��� ������Ʈ + �̵� ���� ������Ʈ
{
	//ī�޶� ȸ�� ��� ���
	XMMATRIX camRotationMatrix = XMMatrixRotationRollPitchYaw(this->rot.x, this->rot.y, this->rot.z);
	//ī�޶� Ÿ�� ���� ��� - ī�޶� ���� ���� , ȸ�� ���
	XMVECTOR camTarget = XMVector3TransformCoord(this->DEFAULT_FORWARD_VECTOR, camRotationMatrix);
	//ī�޶� ���� ��ġ�� �ٸ� ���� ���� ����
	camTarget += this->posVector;
	//ī�޶� ���� ������ ���� ������ ����
	XMVECTOR upDir = XMVector3TransformCoord(this->DEFAULT_UP_VECTOR, camRotationMatrix);
	//�� ��� ����.
	this->viewMatrix = XMMatrixLookAtLH(this->posVector, camTarget, upDir);

	this->UpdateDirectionVectors();
}
