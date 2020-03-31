#pragma once
#include "RenderableGameObject.h"

class Light : public RenderableGameObject {
public:
	bool Initialize();

	DirectX::XMFLOAT3 lightColor = DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f);
	float lightStrength = 1.0f;
	float attenuation_a = 1.0f;
	float attenuation_b = 0.1f;
	float attenuation_c = 0.1f;
};