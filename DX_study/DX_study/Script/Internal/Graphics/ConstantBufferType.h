#pragma once
#include <DirectXMath.h>

struct CB_VS_vertexshader {
	DirectX::XMMATRIX wvpMatrix;
	DirectX::XMMATRIX worldMatrix;
};

struct CB_VS_vertexshader_2d {
	DirectX::XMMATRIX wvpMatrix;
};

struct CB_VS_boneData {
	static const int MAX_BONE = 100;
	DirectX::XMMATRIX boneTransform[MAX_BONE];
};

//8 bytes -> 16 bytes(padding) -> 데이터 정렬 반드시 해야 함. 변수 위치 중요

struct CB_PS_light {
	DirectX::XMFLOAT3 position;
	float range;

	DirectX::XMFLOAT3 forwardVector;
	float spotAngle;

	DirectX::XMFLOAT3 color;
	float strength;

	DirectX::XMFLOAT3 ambientLightColor;
	float ambientLightStrength;

	DirectX::XMFLOAT3 attenuation;
	float pad;
};