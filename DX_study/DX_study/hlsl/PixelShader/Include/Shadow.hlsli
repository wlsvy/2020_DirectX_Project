#ifndef PIXELSHADER_SHADOW_HLSLI
#define PIXELSHADER_SHADOW_HLSLI

#include "Common.hlsli"

SamplerComparisonState cmpSampler
{
   // sampler state
    Filter = COMPARISON_MIN_MAG_MIP_LINEAR;
    AddressU = MIRROR;
    AddressV = MIRROR;
 
   // sampler comparison state
    ComparisonFunc = LESS_THAN;
};

static const float ShadowMapDepthBias = 0.01011f;

float CompareShadowMapDepth(float2 uv, float depth)
{
    return step(shadowMap.Sample(LinearWrap, uv), depth).x;

}

float LerpShadowMapDepth(float2 size, float2 uv, float depth)
{
    float2 texelSize = float2(1.0f, 1.0f) / size * softShadowInterpoloateBias;
    
    float lb = CompareShadowMapDepth(uv + texelSize * float2(0.0f, 0.0f), depth);
    float lt = CompareShadowMapDepth(uv + texelSize * float2(0.0f, 1.0f), depth);
    float rb = CompareShadowMapDepth(uv + texelSize * float2(1.0f, 0.0f), depth);
    float rt = CompareShadowMapDepth(uv + texelSize * float2(1.0f, 1.0f), depth);
    
    return (lb + lt + rb + rt) * 0.25;

}

float PCF(float2 size, float2 uv, float depth)
{
    float result = 0.0f;
    [unroll(50)]
    for (float x = -2; x <= 2; x += 1.0f)
    {
        for (float y = -2; y <= 2; y += 1.0f)
        {
            float2 offset = float2(x, y) / size *  softShadowPCFBias;
            result += LerpShadowMapDepth(size, uv + offset, depth);
            //result += CompareShadowMapDepth(uv + offset, depth);
        }
    }
    
    return result * 0.04;
}

float2 CalculateShadowPCF(int lightIndex, float4 lightSpacePos)
{
    half linearDepth = lightSpacePos.z * 0.02;
    lightSpacePos.xyz /= lightSpacePos.w;
    
    float lightDepth = 1 - linearDepth + ShadowMapDepthBias;
    float2 projectTexCoord = float2(0.5f, -0.5f) * lightSpacePos.xy + 0.5f;
        
    if (projectTexCoord.x <= 0.0f || projectTexCoord.x >= 1.0f || projectTexCoord.y <= 0.0f || projectTexCoord.y >= 1.0f || lightDepth < 0.0f)
    {
        return half2(1.0f, linearDepth);
    }
    
    return half2(PCF(float2(1024, 768), projectTexCoord, lightDepth), linearDepth);
}

float2 CalculateShadow(int lightIndex, float4 lightSpacePos)
{
    half linearDepth = lightSpacePos.z * 0.02;
    lightSpacePos.xyz /= lightSpacePos.w;
    
    float lightDepth = 1 - linearDepth + ShadowMapDepthBias;
    float2 projectTexCoord = float2(0.5f, -0.5f) * lightSpacePos.xy + 0.5f;
        
    if (projectTexCoord.x <= 0.0f || projectTexCoord.x >= 1.0f || projectTexCoord.y <= 0.0f || projectTexCoord.y >= 1.0f || lightDepth < 0.0f)
    {
        return half2(1.0f, linearDepth);
    }
    
    return float2(CompareShadowMapDepth(projectTexCoord, lightDepth), linearDepth);

    return float2(step(shadowMap.Sample(AnisotropicWrap, projectTexCoord).x, lightDepth), linearDepth);
}

#endif 