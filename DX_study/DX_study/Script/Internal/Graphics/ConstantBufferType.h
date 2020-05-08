#pragma once
#include <DirectXMath.h>

struct GpuObjectBuffer {
	DirectX::XMMATRIX wvpMatrix;
	DirectX::XMMATRIX worldMatrix;
};

struct GpuBoneBuffer {
	static const int MAX_BONE = 100;
	DirectX::XMMATRIX boneTransform[MAX_BONE];
};

//8 bytes -> 16 bytes(padding) -> 데이터 정렬 반드시 해야 함. 변수 위치 중요

struct GpuSpotLightBuffer {

	DirectX::XMFLOAT3 position;
	float range;

	DirectX::XMFLOAT3 forwardVector;
	float spotAngle;

	DirectX::XMFLOAT3 color;
	float strength;

	DirectX::XMFLOAT3 attenuation;
	float conePlaneD;

	DirectX::XMMATRIX vpMat;

};

struct CB_DirectionalLight {
	DirectX::XMFLOAT3 color;
	float strength;

	DirectX::XMFLOAT3 forwardVector;
	float pad;
};

struct GpuAmbientLightBuffer {
	DirectX::XMFLOAT3 ambientLightColor = DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f);
	float ambientLightStrength = 0.2f;
};

struct GpuMaterialBuffer {
	DirectX::XMFLOAT4 color;

	float NormalIntensity;
	float MetalIntensity;
	float RoughnessIntensity;
	float SpecularIntensity;
};

struct GpuDownSampleBuffer {
	float threshold = 0.5f;;
	DirectX::XMFLOAT3 pad;
};

struct GpuBlurBuffer {
	static const UINT GAUSSIAN_RADIUS = 7;

	float coefficients[GAUSSIAN_RADIUS + 1];

	int radius;
	int direction;
	DirectX::XMFLOAT2 pad;
};



struct GpuSceneBuffer {
	DirectX::XMVECTOR CamPosition;
	DirectX::XMVECTOR CameraForward;

	float ElapsedTime;
	float DeltaTime;
	DirectX::XMFLOAT2 pad0;

	DirectX::XMFLOAT3 ambientLightColor = DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f);
	float ambientLightStrength = 0.2f;

	DirectX::XMMATRIX InverseViewMat;
	DirectX::XMMATRIX InverseProjMat;

	float softShadowPCFIter = 2.0f;
	float softShadowPCFBias = 2.0f;
	float softShadowInterpoloateBias = 1.0f;
	float pad1;

	float ssao_Strength = 1.0f;
	float ssao_Radius = 0.01f;
	float ssao_Bias = 1.25f;
	float ssao_Scale = 1.0f;

	DirectX::XMFLOAT3 VolumetricLightVar = DirectX::XMFLOAT3(0.04f, 0.1f, 0.2f);
	int VolumetricLightSampleCount = 60;
};

struct GpuFurBuffer {
	int FurLayer = 20;
	float FurLength = 7.0f;
	float FurOpacityThreshold = 0.6f;
	float FurOpacity = 0.01f;
	float FurDensity = 5.0f;
	float FurGravity = 0.1f;
	DirectX::XMFLOAT2 pad;
};