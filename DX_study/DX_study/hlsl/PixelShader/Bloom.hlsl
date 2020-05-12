#include "Include/PostProcessHeader.hlsli"

//√‚√≥ : https://www.gamedev.net/articles/programming/graphics/a-simple-and-practical-approach-to-ssao-r2753/

float ComputeOcclusionPixel(in float2 tcoord, in float2 uv, in float3 p, in float3 cnorm)
{
    float3 diff = positionTexture.Sample(PointClamp, tcoord + uv).xyz - p;
    const float3 v = normalize(diff);
    const float d = length(diff) * SSAO_scale;
    return max(0.0, dot(cnorm, v) - SSAO_bias) * (1.0 / (1.0 + d)) * SSAO_strength;
}

const float SSf = 0.7;
#define SSAO_FACTOR 0.3
float ComputeAmbientOcclusion(float3 position, float3 normal, float2 texcoord)
{
    const float2 vec[4] = { float2(1, 0), float2(-1, 0), float2(0, 1), float2(0, -1) };
    float3 p = position;
    float3 n = normal;
    //float2 rand = float2(1, 1);
    float2 rand = getRandom(texcoord);
    float ao = 0.0f;
    float rad = SSAO_radius / p.z;
  
  //**SSAO Calculation**// 
    int iterations = 4;
    for (int i = 0; i < iterations; ++i)
    {
        float2 coord1 = reflect(vec[i], rand) * rad;
        float2 coord2 = float2(coord1.x * SSAO_FACTOR - coord1.y * SSAO_FACTOR, coord1.x * SSAO_FACTOR + coord1.y * SSAO_FACTOR);
    
        ao += ComputeOcclusionPixel(texcoord, coord1 * 0.25, p, n);
        ao += ComputeOcclusionPixel(texcoord, coord2 * 0.5, p, n);
        ao += ComputeOcclusionPixel(texcoord, coord1 * 0.75, p, n);
        ao += ComputeOcclusionPixel(texcoord, coord2, p, n);
    }
  
    return 1 - saturate(ao / (float) iterations * 4.0);
}

struct PS_INPUT
{
    float4 inPosition : SV_POSITION;
    float2 inTexCoord : TEXCOORD;
};

float4 main(PS_INPUT input) : SV_TARGET
{
    float4 position = positionTexture.Sample(PointClamp, input.inTexCoord);
    float3 normal = normalTexture.Sample(PointClamp, input.inTexCoord);
    
    float ambientOcclusionFactor = ComputeAmbientOcclusion(position.xyz, normal.xyz, input.inTexCoord);
    
    return float4(ambientOcclusionFactor.xxx, 1.0f);
}