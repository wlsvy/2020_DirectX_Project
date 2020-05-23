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
	float EmissionIntensity;
};

struct GpuDownSampleBuffer {
	float threshold = 1.2f;
	float cnt;
	DirectX::XMFLOAT2 radius = DirectX::XMFLOAT2(5.0f, 5.0f);
};

struct GpuBlurBuffer {
	static const UINT GAUSSIAN_RADIUS = 7;

	float coefficients[GAUSSIAN_RADIUS + 1];

	int radius;
	int direction;
	DirectX::XMFLOAT2 pad;
};



struct GpuSceneBuffer {
	DirectX::XMFLOAT3 CamPosition;
	float CamViewRange;

	DirectX::XMVECTOR CameraForward;

	float ElapsedTime;
	float DeltaTime;
	float GammaCorrection = 2.2f;
	float HdrExposure = 1.0f;

	DirectX::XMFLOAT3 ambientLightColor = DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f);
	float ambientLightStrength = 0.2f;

	DirectX::XMMATRIX InverseViewMat;
	DirectX::XMMATRIX InverseProjMat;

	float softShadowPCFIter = 2.0f;
	float softShadowPCFBias = 2.0f;
	float softShadowInterpoloateBias = 1.0f;
	UINT SkyBoxMaxMipLevel = 0.0f;

	float ssao_Strength = 0.07f;
	float ssao_Radius = 0.08f;
	float ssao_Bias = 0.16f;
	float ssao_Scale = 1.0f;

	DirectX::XMFLOAT3 VolumetricLightVar = DirectX::XMFLOAT3(0.00f, 0.1f, 0.2f);
	int VolumetricLightSampleCount = 60;

	float bloom_Threshold = 0.0f;
	float bloom_Intensity = 1.0f;
	DirectX::XMFLOAT2 pad2;
};

struct GpuFurBuffer {
	int FurLayer = 20;
	float FurLength = 7.0f;
	float FurOpacityThreshold = 0.6f;
	float FurOpacity = 0.01f;
	float FurDensity = 5.0f;
	float FurGravity = 0.02f;
	DirectX::XMFLOAT2 pad;
};