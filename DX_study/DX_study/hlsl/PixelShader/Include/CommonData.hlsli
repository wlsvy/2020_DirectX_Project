#ifndef PIXELSHADER_COMMONDATA_HLSLI_
#define PIXELSHADER_COMMONDATA_HLSLI_

#include "CommonTypes.hlsli"

Texture2D DeferredRenderingResource0 : register(t0);
Texture2D DeferredRenderingResource1 : register(t1);
Texture2D DeferredRenderingResource2 : register(t2);
Texture2D DeferredRenderingResource3 : register(t3);
Texture2D depthTexture : register(t4);
Texture2D shadowMap : register(t5);
Texture2D ssaoTexture : register(t6);
Texture2D bloomTexture : register(t7);
Texture2D FurShellOpacityMap : register(t8);
Texture2D ComDeferredRenderingResource0 : register(t9);

Texture2D albedoMap : TEXTURE : register(t11);
Texture2D normalMap : TEXTURE : register(t12);
Texture2D metalMap : TEXTURE : register(t13);
Texture2D roughnessMap : TEXTURE : register(t14);
Texture2D specularMap : TEXTURE : register(t15);

Texture2D randomMap : register(t21);
TextureCube skyBoxCube : register(t22);
Texture2D DitheringTexture : register(t23);
TextureCube irradianceTexture : register(t24);
Texture2D specularBRDF_LUT : register(t25);

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
    
    float softShadowPCFIter;
    float softShadowPCFBias;
    float softShadowInterpoloateBias;
    uint SkyboxMaxMipMapLevel;
    
    float SSAO_strength;
    float SSAO_radius;
    float SSAO_bias;
    float SSAO_scale;
    
    // x: scattering coef, y: extinction coef, z: range w: skybox extinction coef
    float3 VolumetricLightVar;
    int VolumetricLightSampleCount;
}

cbuffer CB_SpotLight : register(b1)
{
    SpotLight spotLight;
}

cbuffer Material : register(b2)
{
    float4 materialColor;
    float NormalIntensity;
    float MetalIntensity;
    float RoughnessIntensity;
    float SpecularIntensity;
};

cbuffer FurDate : register(b3)
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