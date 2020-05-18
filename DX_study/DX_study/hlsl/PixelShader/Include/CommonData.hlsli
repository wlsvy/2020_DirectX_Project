#ifndef PIXELSHADER_COMMONDATA_HLSLI_
#define PIXELSHADER_COMMONDATA_HLSLI_

#include "CommonTypes.hlsli"

Texture2D DeferredRenderingResource0 : register(t0);
Texture2D DeferredRenderingResource1 : register(t1);
Texture2D DeferredRenderingResource2 : register(t2);
Texture2D DeferredRenderingResource3 : register(t3);
Texture2D DepthTexture : register(t4);
Texture2D ShadowMap : register(t5);
Texture2D SsaoTexture : register(t6);
Texture2D BloomTexture : register(t7);
Texture2D FurShellOpacityMap : register(t8);
Texture2D CompositionTexture : register(t9);

Texture2D MaterialAlbedoMap : TEXTURE : register(t11);
Texture2D MaterialNormalMap : TEXTURE : register(t12);
Texture2D MaterialMetalMap : TEXTURE : register(t13);
Texture2D MaterialRoughMap : TEXTURE : register(t14);
Texture2D MaterialEmissionMap : TEXTURE : register(t15);

TextureCube SkyboxCubeMap : register(t21);
TextureCube IrradianceCubeMap : register(t22);

Texture2D RandomValueTexture : register(t31);
Texture2D DitheringTexture : register(t32);
Texture2D BRDF_LUT_IBL : register(t33);


cbuffer CB_Scene : register(b0)
{
    float3 CameraPosition;
    float CameraViewRange;
    float3 CameraForward;
    float pad1;
    
    float ElapsedTime;
    float DeltaTime;
    float Gamma;
    float HdrExposure;
    
    float3 AmbientColor;
    float AmbientStrength;
    
    float4x4 InverseViewMatrix;
    float4x4 InverseProjMatrix;
    
    float SoftShadow_PCFIter;
    float SoftShadow_PCFBias;
    float SoftShadow_InterpoloateBias;
    uint Skybox_MaxMipMapLevel;
    
    float SSAO_strength;
    float SSAO_radius;
    float SSAO_bias;
    float SSAO_scale;
    
    float VolumetricLight_ScatterCoef;
    float VolumetricLight_ExtinctionCoef;
    float pad2;
    int VolumetricLight_SampleCount;
    
    float Bloom_Threshold;
    float Bloom_Intensity;
    float2 pad3;
}

cbuffer CB_SpotLight : register(b1)
{
    SpotLight spotLight;
}

cbuffer Material : register(b2)
{
    float4 AlbedoColor;
    float NormalIntensity;
    float MetalIntensity;
    float RoughnessIntensity;
    float EmissionIntensity;
};

cbuffer FurData : register(b3)
{
    int FurLayer;
    float FurLength;
    float FurOpacityThreshold;
    float FurOpacity;
    float FurDensity;
    float FurGravity;
    float2 pad;
}

#endif