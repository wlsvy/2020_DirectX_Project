#ifndef PIXELSHADER_POSTPROCESSHEADER_HLSLI_
#define PIXELSHADER_POSTPROCESSHEADER_HLSLI_

#include "Common.hlsli"

Texture2D positionTexture : register(t0);
Texture2D normalTexture : register(t1);
Texture2D colorTexture : register(t2);
Texture2D matPropertyTexture : register(t3);
Texture2D depthTexture : register(t4);
Texture2D shadowMap : register(t5);

Texture2D randomMap : register(t8);
TextureCube skyBoxCube : register(t9);
Texture2D DitheringTexture : register(t10);
TextureCube irradianceTexture : register(t11);
Texture2D specularBRDF_LUT : register(t12);

float2 getRandom(in float2 uv)
{
    return normalize(randomMap.Sample(LinearWrap, uv).xy * 2.0f - 1.0f);
}

float FogAttenuation(float density, float distance)
{
    return saturate(1 / exp(distance * density));
}

#endif