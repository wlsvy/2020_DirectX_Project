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
    float4 outPosition : SV_POSITION;
    float3 outLocalPos : LOCAL_POSITION;
};

VS_OUTPUT main(VS_INPUT input)
{
    VS_OUTPUT output;
    output.outPosition = mul(float4(input.inPos, 1.0f), wvpMatrix).xyww; // z = 1�� �ǵ���(�� �ϴ� ���� �׻� �� ��鿡 �ֵ���) z = w�� ����
    output.outLocalPos = input.inPos;
    return output;
}