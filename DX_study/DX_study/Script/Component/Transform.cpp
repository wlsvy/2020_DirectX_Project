#include "Transform.h"

#include <DirectXMath.h>
#include "../Internal/Core/InternalHelper.h"
#include "../Internal/Core/ObjectPool.h"
#include "../Internal/Core/GameObject.h"

using DirectX::operator+=;
using DirectX::operator*;

const float POSITION_MAX = 10000.0f;
const float POSITION_MIN = -10000.0f;
const float Deg2Rad = 0.0174533f;	// pi / 180
const float Rad2Deg = 57.2958f;	// 180 / pi

const DirectX::XMVECTOR Transform::DEFAULT_FORWARD_VECTOR = DirectX::XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);
const DirectX::XMVECTOR Transform::DEFAULT_UP_VECTOR = DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
const DirectX::XMVECTOR Transform::DEFAULT_BACKWARD_VECTOR = DirectX::XMVectorSet(0.0f, 0.0f, -1.0f, 0.0f);
const DirectX::XMVECTOR Transform::DEFAULT_LEFT_VECTOR = DirectX::XMVectorSet(-1.0f, 0.0f, 0.0f, 0.0f);
const DirectX::XMVECTOR Transform::DEFAULT_RIGHT_VECTOR = DirectX::XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f);

//불완전함
DirectX::XMFLOAT3 QuatenionToEuler(float x, float y, float z, float w) {
	float roll, pitch, yaw;
	// roll (x-axis rotation)
	float sinr_cosp = +2.0 * (w * x + y * z);
	float cosr_cosp = +1.0 - 2.0 * (x * x + y * y);
	roll = atan2(sinr_cosp, cosr_cosp);

	// pitch (y-axis rotation)
	float sinp = +2.0 * (w * y - z * x);
	if (fabs(sinp) >= 1)
		pitch = copysign(DirectX::XM_PI / 2, sinp); // use 90 degrees if out of range
	else
		pitch = asin(sinp);

	// yaw (z-axis rotation)
	float siny_cosp = +2.0 * (w * z + x * y);
	float cosy_cosp = +1.0 - 2.0 * (y * y + z * z);
	yaw = atan2(siny_cosp, cosy_cosp);

	return DirectX::XMFLOAT3(roll * Rad2Deg, pitch * Rad2Deg, yaw * Rad2Deg);
}

Transform::~Transform() {
	for (auto& child : m_Children) {
		Pool::Destroy(child.lock()->m_GameObject);
	}

	if (auto ptr = m_Parent.lock()) {
		ptr->EraseChild(this);
	}
}

void Transform::UpdateMatrix(const DirectX::XMMATRIX & parentMatrix)
{
	auto scaleMat = DirectX::XMMatrixScaling(scale.x, scale.y, scale.z);
	auto rotMat = DirectX::XMMatrixRotationRollPitchYaw(rotation.x, rotation.y, rotation.z);
	auto posMat = DirectX::XMMatrixTranslation(position.x, position.y, position.z);

	worldMatrix = 
		scaleMat
		* rotMat
		* posMat
		* parentMatrix;

	this->UpdateDirectionVectors(rotMat);
	for (auto& child : m_Children) {
		child.lock()->UpdateMatrix(worldMatrix);
	}
}

const DirectX::XMVECTOR Transform::GetPositionVector() const
{
	return DirectX::XMVectorSet(position.x, position.y, position.z, 0.0f);
}

const DirectX::XMFLOAT3 & Transform::GetPositionFloat3() const
{
	return this->position;
}

const DirectX::XMVECTOR Transform::GetRotationVector() const
{
	return DirectX::XMVectorSet(rotation.x, rotation.y, rotation.z, 0.0f);
}

const DirectX::XMFLOAT3 & Transform::GetRotationFloat3() const
{
	return this->rotation;
}

const DirectX::XMVECTOR & Transform::GetQuaternionVector() const
{
	return quaternion;
}

const DirectX::XMFLOAT4 Transform::GetQuaternionFloat4() const
{
	DirectX::XMFLOAT4 quaternionfloat4;
	DirectX::XMStoreFloat4(&quaternionfloat4, quaternion);
	return quaternionfloat4;
}

const DirectX::XMFLOAT3 & Transform::GetScaleFloat3() const
{
	return this->scale;
}

void Transform::SetPosition(const DirectX::XMVECTOR & pos)
{
	XMStoreFloat3(&this->position, pos);
}

void Transform::SetPosition(const DirectX::XMFLOAT3 & pos)
{

	this->position = pos;
}

void Transform::SetPosition(float x, float y, float z)
{
	this->position = DirectX::XMFLOAT3(x, y, z);
}

void Transform::translate(const DirectX::XMVECTOR & _pos)
{
	DirectX::XMFLOAT3 translation;
	DirectX::XMStoreFloat3(&translation, _pos);
	position.x += translation.x;
	position.y += translation.y;
	position.z += translation.z;
}

void Transform::translate(float x, float y, float z)
{
	this->position.x += x;
	this->position.y += y;
	this->position.z += z;
}

void Transform::SetRotation(const DirectX::XMFLOAT3 & rot)
{
	rotation = rot;
	quaternion = DirectX::XMQuaternionRotationRollPitchYaw(rotation.x * Deg2Rad, rotation.y * Deg2Rad, rotation.z * Deg2Rad);
}

void Transform::SetRotation(float x, float y, float z)
{
	rotation = DirectX::XMFLOAT3(x, y, z);
	quaternion = DirectX::XMQuaternionRotationRollPitchYaw(x * Deg2Rad, y * Deg2Rad, z * Deg2Rad);
}

void Transform::SetRotation(const DirectX::XMVECTOR & _quat)
{
	this->quaternion = _quat;
	//quaternion -> euler 변경할 적합한 방법이 안 떠오름
	DirectX::XMFLOAT4 quatFloat;
	DirectX::XMStoreFloat4(&quatFloat, quaternion);
	rotation = QuatenionToEuler(quatFloat.x, quatFloat.y, quatFloat.z, quatFloat.w);
}

void Transform::rotate(const DirectX::XMVECTOR & _quaternion)
{
	quaternion = DirectX::XMQuaternionMultiply(_quaternion, quaternion);

	DirectX::XMFLOAT4 quatFloat;
	DirectX::XMStoreFloat4(&quatFloat, quaternion);
	rotation = QuatenionToEuler(quatFloat.x, quatFloat.y, quatFloat.z, quatFloat.w);
}

void Transform::rotate(const DirectX::XMFLOAT3 & rot)
{
	rotation.x += rot.x;
	rotation.y += rot.y;
	rotation.z += rot.z;
	quaternion = DirectX::XMQuaternionRotationRollPitchYaw(rotation.x * Deg2Rad, rotation.y * Deg2Rad, rotation.z * Deg2Rad);
}

void Transform::rotate(float x, float y, float z)
{
	rotation.x += x;
	rotation.y += y;
	rotation.z += z;
	quaternion = DirectX::XMQuaternionRotationRollPitchYaw(rotation.x * Deg2Rad, rotation.y * Deg2Rad, rotation.z * Deg2Rad);
}

void Transform::SetScale(float xScale, float yScale, float zScale)
{
	scale.x = xScale;
	scale.y = yScale;
	scale.z = zScale;
}

void Transform::SetScale(DirectX::XMFLOAT3 _scale)
{
	scale.x = _scale.x;
	scale.y = _scale.y;
	scale.z = _scale.z;
}

void Transform::SetLookAtPos(DirectX::XMFLOAT3 lookAtPos)
{
	//lookatpos 와 cam pos가 동일한지 확인. 이 둘을 같을 수가 없음.
	if (lookAtPos.x == position.x && lookAtPos.y == position.y && lookAtPos.z == position.z)
		return;

	lookAtPos.x = position.x - lookAtPos.x;
	lookAtPos.y = position.y - lookAtPos.y;
	lookAtPos.z = position.z - lookAtPos.z;

	//pitch 각도 구하기
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

void Transform::UpdateDirectionVectors(const DirectX::XMMATRIX & rotationMat) {
	vec_forward = DirectX::XMVector3TransformCoord(DEFAULT_FORWARD_VECTOR, rotationMat);
	vec_backward = vec_forward * -1;
	vec_left = DirectX::XMVector3TransformCoord(DEFAULT_LEFT_VECTOR, rotationMat);
	vec_right = vec_left * -1;
	vec_upward = DirectX::XMVector3TransformCoord(DEFAULT_UP_VECTOR, rotationMat);
}

void Transform::SetChild(const std::shared_ptr<Transform>& child)
{
	if (!HaveChildTransform(child.get())) {
		m_Children.push_back(child);
	}
}

void Transform::EraseChild(Transform* child)
{
	for (auto iter = m_Children.begin(); iter != m_Children.end();) {
		if (iter->expired() ||
			iter->lock().get() == child ) 
		{
			iter = m_Children.erase(iter);
			continue;
		}
		iter++;
	}
}

void Transform::OnGui()
{
	//ImGui::DragFloat3("Position", &position.x, 0.1f, POSITION_MIN, POSITION_MAX);
	////ImGui::DragFloat4("Quaternion", &quaternion.x, 0.1f, -3.0f, 3.0f);
	//ImGui::DragFloat3("Euler Angle", &rotation.x, 0.1f, -1000.0f, 1000.0f);
	//ImGui::DragFloat3("Scale", &scale.x, 0.1f, -1000.0f, 1000.0f);
}

std::shared_ptr<Transform> Transform::GetParent()
{
	return m_Parent.lock();
}

std::shared_ptr<Transform> Transform::GetChild(int index)
{
	return m_Children[index].lock();
}

int Transform::GetChildNum()
{
	return m_Children.size();
}

void Transform::SetParent(const std::shared_ptr<Transform> & transform)
{
	auto parent = GetParent();
	auto* target = transform.get();

	if (transform.get() == this ||
		transform == parent ||
		HaveChildTransform(target))
	{
		return;
	}

	if (target == nullptr) {
		if (parent == Core::GetWorldTransform()) {
			return;
		}
		Core::GetWorldTransform()->SetChild(m_GameObject->GetTransformPtr());
		parent->EraseChild(this);
		m_Parent = Core::GetWorldTransform();
		return;
	}

	if (parent != nullptr)
	{ 
		parent->EraseChild(this);
	}

	transform->SetChild(m_GameObject->GetTransformPtr());
	m_Parent = transform;
}

bool Transform::HaveChildTransform(Transform * _transform)
{
	for (auto& c : m_Children) {
		if (c.lock().get() == _transform) {
			return true;
		}
	}
	return false;
}
