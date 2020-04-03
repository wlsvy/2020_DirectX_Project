#pragma once
#include <DirectXMath.h>
#include "GameObject.h"
using namespace DirectX;

class Camera : public GameObject {
public:
	Camera();
	void UpdateViewMatrix();
	void SetProjectionValues(float fovDegrees, float aspectRatio, float nearZ, float farZ);

	const DirectX::XMMATRIX & GetViewMatrix() const { return viewMatrix; }
	const DirectX::XMMATRIX & GetProjectionMatrix() const { return projectionMatrix; }
	const DirectX::XMMATRIX & GetViewProjectionMatrix() const { return viewProjectionMatrix; }

private:
	XMMATRIX viewMatrix;
	XMMATRIX projectionMatrix;
	XMMATRIX viewProjectionMatrix;
};