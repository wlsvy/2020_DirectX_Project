#pragma once
#include <DirectXMath.h>
#include "../Internal/Core/GameObject.h"

class Light : public GameObject {
public:
	Light() : GameObject("Light") {}

	DirectX::XMFLOAT3 lightColor = DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f);
	float lightStrength = 1.0f;

	DirectX::XMFLOAT3 attenuation = DirectX::XMFLOAT3(1.0f, 0.1f, 0.1f);

	DirectX::XMFLOAT3 lightDir = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);
	float range = 10.0f;
	float cone = 30.0f;
};