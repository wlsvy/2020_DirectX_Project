#ifndef PIXELSHADER_SHADOW_HLSLI
#define PIXELSHADER_SHADOW_HLSLI

#include "PostProcessHeader.hlsli"

SamplerComparisonState cmpSampler
{
   // sampler state
    Filter = COMPARISON_MIN_MAG_MIP_LINEAR;
    AddressU = MIRROR;
    AddressV = MIRROR;
 
   // sampler comparison state
    ComparisonFunc = LESS_THAN;
};
sampler LinearMirror
{
    Filter = COMPARISON_MIN_MAG_MIP_LINEAR;
    AddressU = MIRROR;
    AddressV = MIRROR;
};


float ShadowMapDepthBias = 0.001f;

float CompareShadowMapDepth(float2 uv, float depth)
{
    return step(shadowMap.Sample(PointClamp, uv), depth);

}

float LerpShadowMapDepth(float2 size, float2 uv, float depth)
{
    float2 texelSize = float2(1.0f, 1.0f) / size * 3.0;
    
    float lb = CompareShadowMapDepth(uv + texelSize * float2(0.0f, 0.0f), depth);
    float lt = CompareShadowMapDepth(uv + texelSize * float2(0.0f, 1.0f), depth);
    float rb = CompareShadowMapDepth(uv + texelSize * float2(1.0f, 0.0f), depth);
    float rt = CompareShadowMapDepth(uv + texelSize * float2(1.0f, 1.0f), depth);
    
    return (lb + lt + rb + rt) * 0.25;

}

float PCF(float2 size, float2 uv, float depth)
{
    float result = 0.0f;
    for (float x = -2; x <= 2; x += 1.0f)
    {
        for (float y = -2; y <= 2; y += 1.0f)
        {
            float2 offset = float2(x, y) / size * 5.0f;
            result += LerpShadowMapDepth(size, uv + offset, depth);
        }
    }
    
    return result * 0.04;

}

float4 CalculateShadowPCF(int lightIndex, float4 lightSpacePos)
{
    float lightDepth = 1 - lightSpacePos.z / spotLight.Range + 0.0101f;
    
    float2 projectTexCoord = 0.5f * lightSpacePos.xy / lightSpacePos.w + 0.5f;
    projectTexCoord.y = 1.0f - projectTexCoord.y;
        
    if (projectTexCoord.x <= 0.0f || projectTexCoord.x >= 1.0f || projectTexCoord.y <= 0.0f || projectTexCoord.y >= 1.0f || lightDepth > 1.0f)
    {
        return float4(1.0f, 1.0f, 1.0f, 1.0f);
    }
    
    //return float4((1 - shadowMap.SampleCmpLevelZero(cmpSampler, projectTexCoord, lightDepth)).xxx, 1.0f);
    //return float4(CompareShadowMapDepth(projectTexCoord, lightDepth).xxx, 1.0f);
    return float4(PCF(float2(1024, 768), projectTexCoord, lightDepth).xxx, 1.0f);
    //return float4(LerpShadowMapDepth(float2(1024, 768), projectTexCoord, lightDepth).xxx, 1.0f);
}

float4 CalculateShadow(int lightIndex, float4 lightSpacePos)
{
    float lightDepth = 1 - lightSpacePos.z / spotLight.Range + 0.0001f;
    
    float2 projectTexCoord = 0.5f * lightSpacePos.xy / lightSpacePos.w + 0.5f;
    projectTexCoord.y = 1.0f - projectTexCoord.y;
        
    if (projectTexCoord.x <= 0.0f || projectTexCoord.x >= 1.0f || projectTexCoord.y <= 0.0f || projectTexCoord.y >= 1.0f || lightDepth > 1.0f)
    {
        return float4(1.0f, 1.0f, 1.0f, 1.0f);
    }
    
    float shadowMapDepth = shadowMap.Sample(LinearWrap, projectTexCoord).r;
    
    if (lightDepth >= shadowMapDepth)
    {
        return float4(1.0f, 1.0f, 1.0f, 1.0f);
    }
    return float4(0.0f, 0.0f, 0.0f, 0.0f);
}

#endif 