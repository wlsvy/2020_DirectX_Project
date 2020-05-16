#pragma pack_matrix(row_major)

cbuffer perObjectBuffer : register(b0)
{
    float4x4 wvpMatrix;
    float4x4 worldMatrix;
    float4x4 vpMatrix;
};

struct VS_INPUT
{
    float3 inPos : POSITION;
    float2 inTexCoord : TEXCOORD;
    float3 inNormal : NORMAL;
    float4 inTangent : TANGENT0;
};

struct VS_OUTPUT
{
    float4 outPosition : SV_Position;
    float2 outTexCoord : TEXCOORD0;
    float3 outNormal : NORMAL;
    float3 outWorldPos : WORLD_POSITION;
    float3 outTangent : TANGENT;
    int outOther : TEXCOORD1;
};

VS_OUTPUT main(VS_INPUT input)
{
    VS_OUTPUT output;
    output.outPosition = mul(float4(input.inPos, 1.0f), wvpMatrix);
    output.outTexCoord = input.inTexCoord;
    output.outNormal = normalize(mul(input.inNormal, (float3x3) worldMatrix));
    output.outWorldPos = mul(float4(input.inPos, 1.0f), worldMatrix);
    output.outTangent = normalize(mul(input.inTangent.xyz, (float3x3) worldMatrix));
    output.outOther = 0;
    return output;
}