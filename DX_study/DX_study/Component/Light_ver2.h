#pragma once
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
	Light_ver2(const COMPONENT_INIT_DESC & desc);
	virtual LIGHT_INFO_DESC GetInfoDesc() abstract;

	float lightStrength = 1.0f;
	DirectX::XMFLOAT3 lightColor = DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f);
	const COMPONENT_TYPE mType = COMPONENT_LIGHT;
protected:
	Transform * const transform;
};

class DirectionalLight : public Light_ver2 {
public:
	DirectionalLight(const COMPONENT_INIT_DESC& desc);
	LIGHT_INFO_DESC GetInfoDesc() override;

	LIGHT_TYPE lightType = DIRECTIONAL_LIGHT;
};

class SpotLight : public Light_ver2 {
public:
	SpotLight(const COMPONENT_INIT_DESC& desc);
	LIGHT_INFO_DESC GetInfoDesc() override;

	LIGHT_TYPE lightType = SPOT_LIGHT;
	DirectX::XMFLOAT3 Attentuation = DirectX::XMFLOAT3(0.10f, 0.01f, 0.00f);
	float Range = 30.0f;
	float SpotAngle = 30.0f;
};

class PointLight : public Light_ver2 {
public:
	PointLight(const COMPONENT_INIT_DESC& desc);
	LIGHT_INFO_DESC GetInfoDesc() override;

	LIGHT_TYPE lightType = POINT_LIGHT;
	DirectX::XMFLOAT3 Attentuation = DirectX::XMFLOAT3(0.10f, 0.01f, 0.00f);
	float Range = 30.0f;
};

//class Light : public RenderableGameObject {
//public:
//	bool Initialize(ID3D11Device * device, ID3D11DeviceContext * deviceContext, ConstantBuffer<CB_VS_vertexshader> & cb_vs_vertexshader);
//
//	DirectX::XMFLOAT3 lightColor = DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f);
//	float lightStrength = 1.0f;
//	float attenuation_a = 1.0f;
//	float attenuation_b = 0.1f;
//	float attenuation_c = 0.1f;
//};
//
//struct Light_ver2 {
//	XMFLOAT4 Ambient;
//	XMFLOAT4 Diffuse;
//	XMFLOAT4 Specular;
//};
//
//struct DirectionalLight : public Light_ver2 {
//	DirectionalLight() { ZeroMemory(this, sizeof(this)); }
//
//	XMFLOAT3 Direction;
//	float Pad;//필요하다면 나중에 빛들의 배열을 설정할 수 있도록 float 자리 추가.
//};
//
//struct PointLight : public Light_ver2 {
//	PointLight() { ZeroMemory(this, sizeof(this)); }
//
//	XMFLOAT3 Position;	//(Position, Range) 형태로 4차원 벡터에 넣는다.
//	float Range;
//	XMFLOAT3 Att;		//(A0, A1, A2, pad) 형태로 4차원 벡터에 넣는다.
//	float Pad;
//};
//
//struct SpotLight : public Light_ver2 {
//	SpotLight() { ZeroMemory(this, sizeof(this)); }
//
//	XMFLOAT3 Position;
//	float Range;
//
//	XMFLOAT3 Direction;
//	float Spot;
//
//	XMFLOAT3 Att;
//	float Pad;
//};