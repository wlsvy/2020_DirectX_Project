#pragma pack_matrix(row_major)

cbuffer perObjectBuffer : register(b0)
{
    float4x4 wvpMatrix;
    float4x4 worldMatrix;
    float4x4 vpMatrix;
};
//������� : ���̴��� ������ �� �ִ� �پ��� �ڷḦ �����ϴ� ������ �ڷ� ���
//��� ������ �ڷ�� �������� �ٲ�� ���� �ƴϴ�. �׷��� c++ ���� ���α׷��� ȿ�� �����ӿ�ũ�� ���ؼ� ��� ������ ������ ����������� ������ �� �ִ�.
//��� ���۴� c++���� ���α׷� �ڵ�� ȿ�� �ڵ尡 �ǻ�����ϴ� ������ �ȴ�.
struct VS_INPUT
{
    float3 inPos : POSITION;
    //float3 inColor : COLOR;
    float2 inTexCoord : TEXCOORD;
    float3 inNormal : NORMAL;
};

struct VS_OUTPUT
{
	float4 outPosition : SV_POSITION;
    //float3 outColor : COLOR;
    float2 outTexCoord : TEXCOORD;
    float3 outNormal : NORMAL;
    float3 outWorldPos : WORLD_POSITION;
};

VS_OUTPUT main(VS_INPUT input)
{
    VS_OUTPUT output;
    output.outPosition = mul(float4(input.inPos, 1.0f), wvpMatrix);
    output.outTexCoord = input.inTexCoord;
    output.outNormal = mul(float4(input.inNormal, 0.0f), worldMatrix); //����� ����ȭ�� �ʿ� ���� ���� �ְڴ�.���ʿ� ������ ���� ����ȭ�� ������ �ٵ�
    output.outWorldPos = mul(float4(input.inPos, 1.0f), worldMatrix);

    output.outPosition = mul(output.outPosition, 10.0f);
    return output;
}