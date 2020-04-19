#pragma once
#include "Component.h"

#include <vector>
#include <DirectXMath.h>


class Scene;

class Transform : public Component {
	friend class Scene;
public:
	COMPONENT_CONSTRUCTOR(Transform, Component)
	~Transform();
	void OnGui() override;

	const DirectX::XMVECTOR GetPositionVector() const;
	const DirectX::XMFLOAT3 & GetPositionFloat3() const;
	const DirectX::XMVECTOR GetRotationVector() const;
	const DirectX::XMFLOAT3 & GetRotationFloat3() const;
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

	const DirectX::XMVECTOR & GetForwardVector() const { return vec_forward; }
	const DirectX::XMVECTOR & GetRightVector() const { return vec_right; }
	const DirectX::XMVECTOR & GetLeftVector() const { return vec_left; }
	const DirectX::XMVECTOR & GetBackwardVector() const { return vec_backward; }
	const DirectX::XMVECTOR & GetUpwardVector() const { return vec_upward; }

	const DirectX::XMMATRIX & GetWorldMatrix() const { return worldMatrix; }

	std::shared_ptr<Transform> GetParent();
	std::shared_ptr<Transform> GetChild(int index);
	int GetChildNum();

	void SetParent(const std::shared_ptr<Transform> & transform);
	bool HaveChildTransform(Transform * _transform);

	DirectX::XMFLOAT3 position = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);
	DirectX::XMFLOAT3 rotation = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);
	DirectX::XMFLOAT3 scale = DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f);

	static const DirectX::XMVECTOR DEFAULT_FORWARD_VECTOR;
	static const DirectX::XMVECTOR DEFAULT_UP_VECTOR;
	static const DirectX::XMVECTOR DEFAULT_BACKWARD_VECTOR;
	static const DirectX::XMVECTOR DEFAULT_LEFT_VECTOR;
	static const DirectX::XMVECTOR DEFAULT_RIGHT_VECTOR;

protected:
	void UpdateMatrix(const DirectX::XMMATRIX & parentMatrix);
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