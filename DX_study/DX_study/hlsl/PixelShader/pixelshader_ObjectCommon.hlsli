#include "pixelshader_Common.hlsli"

struct PS_INPUT
{
    float4 inPosition : SV_POSITION0;
    float2 inTexCoord : TEXCOORD0;
    float3 inNormal : NORMAL0;
    float3 inWorldPos : WORLD_POSITION0;
    float3 inTangent : TANGENT;
    float inDepth : DEPTH;
};

struct PS_OUTPUT
{
    float4 pos : WORLD_POSITION;
    float4 normal : NORMAL;
    float4 color : COLOR0;
    float4 light : COLOR1;
    float4 depth : COLOR2;
};

cbuffer Material : register(b2)
{
    float4 materialColor;
};

Texture2D objTexture : TEXTURE : register(t0);
Texture2D normalMap : TEXTURE : register(t1);
Texture2D shadowMap : TEXTURE : register(t3);



float3 CalcPerPixelNormal(float2 vTexcoord, float3 vVertNormal, float3 vVertTangent)
{
    float3 vVertBinormal = normalize(cross(vVertTangent, vVertNormal));
    float3x3 mTangentSpaceToWorldSpace = float3x3(vVertTangent, vVertBinormal, vVertNormal);

    float3 vBumpNormal = (float3) normalMap.Sample(LinearWrap, vTexcoord);
    vBumpNormal = 2.0f * vBumpNormal - 1.0f;

    return mul(vBumpNormal, mTangentSpaceToWorldSpace);
}