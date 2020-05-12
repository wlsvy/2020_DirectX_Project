#ifndef PIXELSHADER_POSTPROCESSHEADER_HLSLI_
#define PIXELSHADER_POSTPROCESSHEADER_HLSLI_

#include "Common.hlsli"

Texture2D positionTexture : register(t0);
Texture2D normalTexture : register(t1);
Texture2D colorTexture : register(t2);
Texture2D matPropertyTexture : register(t3);
Texture2D depthTexture : register(t4);
Texture2D shadowMap : register(t5);
Texture2D ssaoTexture : register(t6);
Texture2D bloomTexture : register(t7);


Texture2D randomMap : register(t21);
TextureCube skyBoxCube : register(t22);
Texture2D DitheringTexture : register(t23);
TextureCube irradianceTexture : register(t24);
Texture2D specularBRDF_LUT : register(t25);

float2 getRandom(in float2 uv)
{
    return normalize(randomMap.Sample(LinearWrap, uv).xy * 2.0f - 1.0f);
}

float FogAttenuation(float density, float distance)
{
    return saturate(1 / exp(distance * density));
}

#endif