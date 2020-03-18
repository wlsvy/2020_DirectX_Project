#pragma once
#include "RenderableGameObject.h"

class Light : public RenderableGameObject {
public:
	bool Initialize(Model * _model);

	DirectX::XMFLOAT3 lightColor = DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f);
	float lightStrength = 1.0f;
	float attenuation_a = 1.0f;
	float attenuation_b = 0.1f;
	float attenuation_c = 0.1f;
};

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