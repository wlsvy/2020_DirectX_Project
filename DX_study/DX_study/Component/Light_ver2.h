#pragma once
#include <DirectXMath.h>

#include "Component.h"

enum LIGHT_TYPE {DIRECTIONAL_LIGHT = 0, POINT_LIGHT = 1, SPOT_LIGHT = 2};

struct LIGHT_INFO_DESC {
	const LIGHT_TYPE *lightType;
	const DirectX::XMFLOAT3 *lightPos;
	const DirectX::XMFLOAT3 *lightVec;
	const DirectX::XMFLOAT3 *lightColor;
	const DirectX::XMFLOAT3 *lightAttentuation;
	const float *lightRange;
	const float *lightStrength;
	const float *spotAngle;
};

class Light_ver2 : public Component {
public:
	using ComponentTag = LigthComponentTag;

	Light_ver2(GameObject_v2 & obj);
	virtual LIGHT_INFO_DESC GetInfoDesc() abstract;

	float lightStrength = 1.0f;
	DirectX::XMFLOAT3 lightColor = DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f);
protected:
	Transform * const transform;
};

class DirectionalLight : public Light_ver2 {
public:
	DirectionalLight(GameObject_v2 & obj);
	LIGHT_INFO_DESC GetInfoDesc() override;

	LIGHT_TYPE lightType = DIRECTIONAL_LIGHT;
};

class SpotLight : public Light_ver2 {
public:
	SpotLight(GameObject_v2 & obj);
	LIGHT_INFO_DESC GetInfoDesc() override;

	LIGHT_TYPE lightType = SPOT_LIGHT;
	DirectX::XMFLOAT3 Attentuation = DirectX::XMFLOAT3(0.10f, 0.01f, 0.00f);
	float Range = 30.0f;
	float SpotAngle = 30.0f;
};

class PointLight : public Light_ver2 {
public:
	PointLight(GameObject_v2 & obj);
	LIGHT_INFO_DESC GetInfoDesc() override;

	LIGHT_TYPE lightType = POINT_LIGHT;
	DirectX::XMFLOAT3 Attentuation = DirectX::XMFLOAT3(0.10f, 0.01f, 0.00f);
	float Range = 30.0f;
};