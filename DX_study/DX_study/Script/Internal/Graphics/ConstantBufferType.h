#pragma once
#include <DirectXMath.h>

struct CB_VS_vertexshader {
	DirectX::XMMATRIX wvpMatrix;
	DirectX::XMMATRIX worldMatrix;
	//float xOffset = 0;	//4bytes
	//float yOffset = 0;	//4bytes
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
	DirectX::XMFLOAT3 dynamicLightPosition;
	float range;

	DirectX::XMFLOAT3 dir;
	float cone;

	DirectX::XMFLOAT3 dynamicLightColor;
	float dynamicLightStrength;

	DirectX::XMFLOAT3 ambientLightColor;
	float ambientLightStrength;

	DirectX::XMFLOAT3 attenuation;
	float pad;
};