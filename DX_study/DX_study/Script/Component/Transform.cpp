#include "Transform.h"

#include "../Internal/Engine/Ui.h"
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
const DirectX::XMVECTOR Transform::DEFAULT_BACKWARD_VECTOR = DirectX::XMVectorSet(0.0f, 0.0f, -1.0f, 0.0f);
const DirectX::XMVECTOR Transform::DEFAULT_UP_VECTOR = DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
const DirectX::XMVECTOR Transform::DEFAULT_DOWN_VECTOR = DirectX::XMVectorSet(0.0f, -1.0f, 0.0f, 0.0f);
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
		Core::Destroy(child.lock()->m_GameObject);
	}

	if (auto ptr = m_Parent.lock()) {
		ptr->EraseChild(this);
	}
}

void Transform::UpdateMatrix(const DirectX::XMMATRIX & parentWorldMatrix, const DirectX::XMVECTOR & parentQuat)
{
	auto quaternion = GetQuaternion();

	auto scaleMat = DirectX::XMMatrixScalingFromVector(scaleVec);
	auto rotMat = DirectX::XMMatrixRotationQuaternion(quaternion);
	auto posMat = DirectX::XMMatrixTranslationFromVector(positionVec);

	m_WorldMatrix =
		scaleMat
		* rotMat
		* posMat
		* parentWorldMatrix;

	m_GlobalPositionVec = DirectX::XMVector3Transform(positionVec, parentWorldMatrix);
	m_GlobalQuaternionVec = DirectX::XMQuaternionMultiply(quaternion, parentQuat);
	m_GlobalLossyScaleVec = CalculateLossyScale();

	this->UpdateDirectionVectors(rotMat);
	for (auto& child : m_Children) {
		child.lock()->UpdateMatrix(m_WorldMatrix, m_GlobalQuaternionVec);
	}
}

DirectX::XMVECTOR Transform::CalculateLossyScale() const
{
	auto globalPosMat = DirectX::XMMatrixTranslationFromVector(m_GlobalPositionVec);
	auto globalRotMat = DirectX::XMMatrixRotationQuaternion(m_GlobalQuaternionVec);
	auto rpMat = globalRotMat * globalPosMat;

	auto inv = DirectX::XMMatrixInverse(nullptr, rpMat);
	auto globalScaleMat = m_WorldMatrix * inv;

	return DirectX::XMVectorSet(
		globalScaleMat.r[0].m128_f32[0], 
		globalScaleMat.r[1].m128_f32[1], 
		globalScaleMat.r[2].m128_f32[2], 
		0.0f);
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
	m_Forward = DirectX::XMVector3TransformCoord(DEFAULT_FORWARD_VECTOR, rotationMat);
	m_Left = DirectX::XMVector3TransformCoord(DEFAULT_LEFT_VECTOR, rotationMat);
	m_Upward = DirectX::XMVector3TransformCoord(DEFAULT_UP_VECTOR, rotationMat);
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
	ImGui::DragFloat3("Position", &position.x, 0.1f, POSITION_MIN, POSITION_MAX);
	ImGui::DragFloat3("Rotation", &rotation.x, 0.1f, -1000.0f, 1000.0f);
	ImGui::DragFloat3("Scale", &scale.x, 0.1f, -1000.0f, 1000.0f);
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
