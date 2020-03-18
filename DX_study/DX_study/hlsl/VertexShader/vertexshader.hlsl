#pragma pack_matrix(row_major)

cbuffer perObjectBuffer : register(b0)
{
    float4x4 wvpMatrix;
    float4x4 worldMatrix;
    float4x4 vpMatrix;
};
//상수버퍼 : 셰이더가 접근할 수 있는 다양한 자료를 저장하는 유연한 자료 블록
//상수 버퍼의 자료는 정점마다 바뀌는 것이 아니다. 그러나 c++ 응용 프로그램은 효과 프레임워크를 통해서 상수 버퍼의 내용을 실행시점에서 변경할 수 있다.
//상수 버퍼는 c++응용 프로그램 코드와 효과 코드가 의사소통하는 수단이 된다.
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
    output.outNormal = mul(float4(input.inNormal, 0.0f), worldMatrix); //여기는 정규화가 필요 없을 수도 있겠다.애초에 들어오는 값이 정규화가 되있을 텐데
    output.outWorldPos = mul(float4(input.inPos, 1.0f), worldMatrix);

    output.outPosition = mul(output.outPosition, 10.0f);
    return output;
}