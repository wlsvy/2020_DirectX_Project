#pragma once
#include <Queue>
#include <DirectXMath.h>
#include <DirectXCollision.h>
#include "../Internal/Core/GameObject.h"
#include "../Component/Renderable.h"

class Camera : public GameObject {
public:
	Camera();
	void UpdateView();
	void SetProjectionValues(float fovDegrees, float aspectRatio, float nearZ, float farZ);

	const DirectX::XMMATRIX & GetViewMatrix() const { return viewMatrix; }
	const DirectX::XMMATRIX & GetProjectionMatrix() const { return projectionMatrix; }
	const DirectX::XMMATRIX & GetViewProjectionMatrix() const { return viewProjectionMatrix; }
	const DirectX::BoundingFrustum & GetViewFrustum() const { return viewFrustum; }

private:
	DirectX::XMMATRIX viewMatrix;
	DirectX::XMMATRIX projectionMatrix;
	DirectX::XMMATRIX viewProjectionMatrix;
	DirectX::BoundingFrustum viewFrustum;

	std::queue<RenderablePair> m_RenderQueue;
};