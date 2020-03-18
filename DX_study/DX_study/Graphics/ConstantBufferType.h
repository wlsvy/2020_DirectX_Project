#pragma once
#include <DirectXMath.h>
#include <vector>

#define MAX_DIRECTIONAL_LIGHT 2
#define MAX_POINT_LIGHT 4
#define MAX_SPOT_LIGHT 4
#define MAX_BONE_NUM 100

struct CB_VS_vertexshader {
	DirectX::XMMATRIX wvpMatrix;
	DirectX::XMMATRIX worldMatrix;
	DirectX::XMMATRIX vpMatrix;
	//float xOffset = 0;	//4bytes
	//float yOffset = 0;	//4bytes
};

struct CB_VS_vertexshader_2d {
	DirectX::XMMATRIX wvpMatrix;
};
//8 bytes -> 16 bytes(padding) -> 데이터 정렬 반드시 해야 함. 변수 위치 중요

struct CB_VS_boneData {
	DirectX::XMMATRIX boneTransform[MAX_BONE_NUM];
};

struct CB_PS_light {
	DirectX::XMFLOAT3 ambientLightColor;
	float ambientLightStrength;
	//여기까지 16바이트

	DirectX::XMFLOAT3 dynamicLightColor;
	float dynamicLightStrength;
	//여기까지 또 16 바이트

	DirectX::XMFLOAT3 dynamicLightPosition;
	
	float dynamicLightAttenuation_a;
	float dynamicLightAttenuation_b;
	float dynamicLightAttenuation_c;
	float pad[2];
};

struct CB_PS_Fog {
	DirectX::XMFLOAT3 fogColor;
	float fogStart;
	float fogRange;
	int fogEnabled;
	int pad[2];
};

struct CB_PS_simpleLight {
	DirectX::XMFLOAT3 ambientLightColor;
	float ambientLightStrength;
};

struct CB_PS_CameraPos {
	DirectX::XMFLOAT3 dynamicCameraPosition;
	float pad;
};

struct CB_PS_ColorArray {
	DirectX::XMFLOAT4 array_Color[4];
	int index = 0;
	DirectX::XMFLOAT3 pad;
};

struct CB_LIGHT_Directional {
	DirectX::XMFLOAT3 lightVec;
	float lightStrength;	//16
	DirectX::XMFLOAT3 lightColor;
	float pad;				//16
};

struct CB_LIGHT_Point{
	DirectX::XMFLOAT3 lightColor;
	float lightStrength;	//16
	DirectX::XMFLOAT3 lightPos;
	float lightRange;		//16
	DirectX::XMFLOAT3 Attentuation = DirectX::XMFLOAT3(0.10f, 0.01f, 0.00f);
	float pad;				//16
};

struct CB_LIGHT_Spot {
	DirectX::XMFLOAT3 lightColor;
	float lightStrength;	//16
	DirectX::XMFLOAT3 lightPos;
	float lightRange;	//16
	DirectX::XMFLOAT3 Attentuation = DirectX::XMFLOAT3(0.10f, 0.01f, 0.00f);
	float spotAngle;	//16
};

struct CB_PS_LightArray {
	CB_LIGHT_Directional directional_light[MAX_DIRECTIONAL_LIGHT];
	CB_LIGHT_Point point_light[MAX_POINT_LIGHT];
	CB_LIGHT_Spot spot_light[MAX_SPOT_LIGHT];
};