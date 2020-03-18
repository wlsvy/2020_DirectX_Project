#include "GameObject3D.h"

void GameObject3D::SetLookAtPos(DirectX::XMFLOAT3 lookAtPos)
{
	//lookatpos �� cam pos�� �������� Ȯ��. �� ���� ���� ���� ����.
	if (lookAtPos.x == pos.x && lookAtPos.y == pos.y && lookAtPos.z == pos.z)
		return;

	lookAtPos.x = pos.x - lookAtPos.x;
	lookAtPos.y = pos.y - lookAtPos.y;
	lookAtPos.z = pos.z - lookAtPos.z;
	
	//pitch ���� ���ϱ�
	float pitch = 0.0f;
	if (lookAtPos.y != 0.0f) {
		const float distance = sqrt(lookAtPos.x * lookAtPos.x + lookAtPos.z * lookAtPos.z);
		pitch = atan(lookAtPos.y / distance);
	}

	float yaw = 0.0f;
	if (lookAtPos.x != 0.0f) {
		yaw = atan(lookAtPos.x / lookAtPos.z);
	}
	if (lookAtPos.z > 0) yaw += DirectX::XM_PI;

	this->SetRotation(pitch, yaw, 0.0f);
}

void GameObject3D::UpdateMatrix()
{
	assert("UpdateMatrix must be override" && 0);
}

void GameObject3D::UpdateDirectionVectors() {
	DirectX::XMMATRIX vecRotationMatirx = DirectX::XMMatrixRotationRollPitchYaw(this->rot.x, this->rot.y, 0.0f);
	this->vec_forward = DirectX::XMVector3TransformCoord(this->DEFAULT_FORWARD_VECTOR, vecRotationMatirx);
	this->vec_backward = DirectX::XMVector3TransformCoord(this->DEFAULT_BACKWARD_VECTOR, vecRotationMatirx);
	this->vec_left = DirectX::XMVector3TransformCoord(this->DEFAULT_LEFT_VECTOR, vecRotationMatirx);
	this->vec_right = DirectX::XMVector3TransformCoord(this->DEFAULT_RIGHT_VECTOR, vecRotationMatirx);

	DirectX::XMMATRIX vecRotationMatirxNoY = DirectX::XMMatrixRotationRollPitchYaw(0.0f, this->rot.y, 0.0f);
	this->vec_forward_noY = DirectX::XMVector3TransformCoord(this->DEFAULT_FORWARD_VECTOR, vecRotationMatirxNoY);
	this->vec_backward_noY = DirectX::XMVector3TransformCoord(this->DEFAULT_BACKWARD_VECTOR, vecRotationMatirxNoY);
	this->vec_left_noY = DirectX::XMVector3TransformCoord(this->DEFAULT_LEFT_VECTOR, vecRotationMatirxNoY);
	this->vec_right_noY = DirectX::XMVector3TransformCoord(this->DEFAULT_RIGHT_VECTOR, vecRotationMatirxNoY);
}

const DirectX::XMVECTOR & GameObject3D::GetForwardVector(bool omitY)
{
	if (omitY) return vec_forward_noY;
	return vec_forward;
}

const DirectX::XMVECTOR & GameObject3D::GetRightVector(bool omitY)
{
	if (omitY) return vec_right_noY;
	return vec_right;
}

const DirectX::XMVECTOR & GameObject3D::GetLeftVector(bool omitY)
{
	if (omitY) return vec_left_noY;
	return vec_left;
}

const DirectX::XMVECTOR & GameObject3D::GetBackwardVector(bool omitY)
{
	if (omitY) return vec_backward_noY;
	return vec_backward;
}