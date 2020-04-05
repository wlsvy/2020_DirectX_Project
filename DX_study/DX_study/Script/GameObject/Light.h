#pragma once
#include <DirectXMath.h>
#include "../Internal/Core/GameObject.h"

class Light : public GameObject {
public:
	Light() : GameObject("Light") {}

	DirectX::XMFLOAT3 lightColor = DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f);
	float lightStrength = 1.0f;
	float attenuation_a = 1.0f;
	float attenuation_b = 0.1f;
	float attenuation_c = 0.1f;
};