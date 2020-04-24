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
};

struct VS_OUTPUT
{
    //float4 outPosition : SV_POSITION;
    
    float2 outTexCoord : TEXCOORD;
    float3 outNormal : NORMAL;
    float3 outWorldPos : WORLD_POSITION;
    float4 outTestPosition : TEST_POSITION;
};

VS_OUTPUT main(VS_INPUT input)
{
    VS_OUTPUT output;
    //output.outPosition = mul(float4(input.inPos, 1.0f), wvpMatrix);
    //output.outTestPosition = mul(float4(input.inPos, 1.0f), wvpMatrix);
    output.outTexCoord = input.inTexCoord;
    output.outNormal = mul(float4(input.inNormal, 0.0f), worldMatrix); //여기는 정규화가 필요 없을 수도 있겠다.애초에 들어오는 값이 정규화가 되있을 텐데
    output.outWorldPos = mul(float4(input.inPos, 1.0f), worldMatrix);
    output.outTestPosition = mul(float4(output.outWorldPos, 1.0f), vpMatrix);
    //output.vpMatrix = vpMatrix;
    //output.wvpMATRIX = wvpMatrix;

    //output.outPosition = mul(output.outPosition, 10.0f);
    return output;
}