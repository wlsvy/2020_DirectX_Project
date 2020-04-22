#pragma once
#include "Component.h"
#include "../Util/Math.h"

#include <vector>

class Scene;

class Transform : public Component {
	friend class Scene;
public:
	COMPONENT_CONSTRUCTOR(Transform, Component)
	~Transform();
	void OnGui() override;
	
	const DirectX::XMVECTOR & GetPositionVector() const		{ return positionVec; }
	const DirectX::XMFLOAT3 & GetPositionFloat3() const		{ return position; }
	const DirectX::XMVECTOR & GetRotationVector() const		{ return rotationVec; }
	const DirectX::XMFLOAT3 & GetRotationFloat3() const		{ return rotation; }
	DirectX::XMVECTOR GetQuaternion() const			{ using DirectX::operator*;  return DirectX::XMQuaternionRotationRollPitchYawFromVector(GetRotationVector() * Math::Deg2Rad); }
	const DirectX::XMFLOAT3 & GetScaleFloat3() const		{ return scale; }

	void SetPosition(const DirectX::XMVECTOR & pos);
	void SetPosition(const DirectX::XMFLOAT3 & pos);
	void SetPosition(float x, float y, float z);

	void translate(const DirectX::XMVECTOR & _pos);
	void translate(float x, float y, float z);

	void SetRotation(const DirectX::XMFLOAT3 & rot);
	void SetRotation(float x, float y, float z);
	void SetRotation(const DirectX::XMVECTOR & _quat);

	void rotate(const DirectX::XMFLOAT3 & rot);
	void rotate(float x, float y, float z);
	void rotate(const DirectX::XMVECTOR & _quaternion);

	void SetScale(float xScale, float yScale, float zScale = 1.0f);
	void SetScale(DirectX::XMFLOAT3 _scale);

	void SetLookAtPos(DirectX::XMFLOAT3 lookAtPos);

	const DirectX::XMVECTOR & GetForwardVector() const { return vec_forward; }
	const DirectX::XMVECTOR & GetRightVector() const { return vec_right; }
	const DirectX::XMVECTOR & GetLeftVector() const { return vec_left; }
	const DirectX::XMVECTOR & GetBackwardVector() const { return vec_backward; }
	const DirectX::XMVECTOR & GetUpwardVector() const { return vec_upward; }

	const DirectX::XMMATRIX & GetWorldMatrix() const { return worldMatrix; }
	const DirectX::XMMATRIX & GetRotationMatrix() const { return DirectX::XMMatrixRotationQuaternion(GetQuaternion()); }

	std::shared_ptr<Transform> GetParent();
	std::shared_ptr<Transform> GetChild(int index);
	int GetChildNum();

	void SetParent(const std::shared_ptr<Transform> & transform);
	bool HaveChildTransform(Transform * _transform);

	union {
		DirectX::XMFLOAT3 position = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);
		DirectX::XMVECTOR positionVec;
	};
	union {
		DirectX::XMFLOAT3 rotation = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);
		DirectX::XMVECTOR rotationVec;
	};
	union {
		DirectX::XMFLOAT3 scale = DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f);
		DirectX::XMVECTOR scaleVec;
	}; 
	union {
		DirectX::XMFLOAT3 globalPosition;
		DirectX::XMVECTOR globalPositionVec;
	};
	union {
		DirectX::XMFLOAT4 globalQuaternion;
		DirectX::XMVECTOR globalQuaternionVec;
	};
	union {
		DirectX::XMFLOAT3 globalScale;
		DirectX::XMVECTOR globalScaleVec;
	};
	
	static const DirectX::XMVECTOR DEFAULT_FORWARD_VECTOR;
	static const DirectX::XMVECTOR DEFAULT_UP_VECTOR;
	static const DirectX::XMVECTOR DEFAULT_BACKWARD_VECTOR;
	static const DirectX::XMVECTOR DEFAULT_LEFT_VECTOR;
	static const DirectX::XMVECTOR DEFAULT_RIGHT_VECTOR;

private:
	void UpdateMatrix(const DirectX::XMMATRIX & parentMatrix);
	void UpdateMatrix(
		const DirectX::XMMATRIX & pos,
		const DirectX::XMVECTOR & parentQuat,
		const DirectX::XMMATRIX & scale);
	void UpdateDirectionVectors(const DirectX::XMMATRIX & rotationMat);

	void SetChild(const std::shared_ptr<Transform> & child);
	void EraseChild(Transform* child);

	DirectX::XMMATRIX worldMatrix = DirectX::XMMatrixIdentity();
	DirectX::XMVECTOR vec_forward;
	DirectX::XMVECTOR vec_left;
	DirectX::XMVECTOR vec_right;
	DirectX::XMVECTOR vec_backward;
	DirectX::XMVECTOR vec_upward;

	std::weak_ptr<Transform> m_Parent;
	std::vector<std::weak_ptr<Transform>> m_Children;
};