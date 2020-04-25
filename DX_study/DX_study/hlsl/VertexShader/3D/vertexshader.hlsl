#pragma pack_matrix(row_major)

cbuffer perObjectBuffer : register(b0)
{
    float4x4 wvpMatrix;
    float4x4 worldMatrix;
    float4x4 vpMatrix;
};

cbuffer ShadowMapData : register(b2)
{
    float4x4 lightVPmatrix;
    float3 lightpos;
    float pad;
}

struct VS_INPUT
{
    float3 inPos : POSITION;
    float2 inTexCoord : TEXCOORD;
    float3 inNormal : NORMAL;
};

struct VS_OUTPUT
{
    float4 outPosition : SV_Position;
    float2 outTexCoord : TEXCOORD0;
    float3 outNormal : NORMAL;
    float3 outWorldPos : WORLD_POSITION;
    float4 outLightPosition : TEXCOORD1;
    float4 outToLightDir : TEXCOORD2;
};

VS_OUTPUT main(VS_INPUT input)
{
    VS_OUTPUT output;
    output.outPosition = mul(float4(input.inPos, 1.0f), wvpMatrix);
    output.outTexCoord = input.inTexCoord;
    output.outNormal = normalize(mul(float4(input.inNormal, 0.0f), worldMatrix));
    output.outWorldPos = mul(float4(input.inPos, 1.0f), worldMatrix);
    //output.outPosition = mul(float4(output.outWorldPos.xyz, 1.0f), lightVPmatrix);


    output.outLightPosition = mul(float4(output.outWorldPos.xyz, 1.0f), lightVPmatrix);
    output.outToLightDir = float4(normalize(lightpos.xyz - output.outWorldPos.xyz), 0.0f);
    return output;
}