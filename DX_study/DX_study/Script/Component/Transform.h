#pragma once
#include <vector>
#include <DirectXMath.h>

#include "Component.h"

class Transform : public Component {
public:
	~Transform();
	void OnGui() override;

	const DirectX::XMVECTOR GetPositionVector() const;
	const DirectX::XMFLOAT3 & GetPositionFloat3() const;
	const DirectX::XMVECTOR GetRotationVector() const;
	const DirectX::XMFLOAT3 & GetRotationFloat3() const;
	const DirectX::XMVECTOR & GetQuaternionVector() const;
	const DirectX::XMFLOAT4 GetQuaternionFloat4() const;
	const DirectX::XMFLOAT3 & GetScaleFloat3() const;

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

	const DirectX::XMVECTOR & GetForwardVector(bool omitY = false);
	const DirectX::XMVECTOR & GetRightVector(bool omitY = false);
	const DirectX::XMVECTOR & GetLeftVector(bool omitY = false);
	const DirectX::XMVECTOR & GetBackwardVector(bool omitY = false);
	const DirectX::XMVECTOR & GetUpwardVector(bool omitY = false);

	const DirectX::XMMATRIX & GetWorldMatrix();

	Transform * GetParent();
	int GetChildNum();
	Transform * GetChild(int index);
	void SetParent(Transform * _transform);
	bool HaveChildTransform(Transform * _transform);

	DirectX::XMVECTOR quaternion = DirectX::XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);

	DirectX::XMFLOAT3 position = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);
	DirectX::XMFLOAT3 rotation = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);
	DirectX::XMFLOAT3 scale = DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f);

	DirectX::XMVECTOR vec_forward;
	DirectX::XMVECTOR vec_left;
	DirectX::XMVECTOR vec_right;
	DirectX::XMVECTOR vec_backward;
	DirectX::XMVECTOR vec_upward;

	bool TRANSFORM_UPDATED = false;

	void UpdateMatrix(DirectX::XMMATRIX & _parentMatrix);
	DirectX::XMMATRIX worldMatrix = DirectX::XMMatrixIdentity();


protected:
	void UpdateDirectionVectors();
	void SetChild(Transform * _transform);
	void EraseChild(Transform * _transform);

	const DirectX::XMVECTOR DEFAULT_FORWARD_VECTOR = DirectX::XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);
	const DirectX::XMVECTOR DEFAULT_UP_VECTOR = DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
	const DirectX::XMVECTOR DEFAULT_BACKWARD_VECTOR = DirectX::XMVectorSet(0.0f, 0.0f, -1.0f, 0.0f);
	const DirectX::XMVECTOR DEFAULT_LEFT_VECTOR = DirectX::XMVectorSet(-1.0f, 0.0f, 0.0f, 0.0f);
	const DirectX::XMVECTOR DEFAULT_RIGHT_VECTOR = DirectX::XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f);

	DirectX::XMVECTOR vec_forward_noY;
	DirectX::XMVECTOR vec_left_noY;
	DirectX::XMVECTOR vec_right_noY;
	DirectX::XMVECTOR vec_backward_noY;
	DirectX::XMVECTOR vec_upward_noY;

	int mChildNum = 0;
	Transform * mParent = nullptr;
	std::vector<Transform*> mChildTransform;
	Transform * mWorldTransform = nullptr;
	
};