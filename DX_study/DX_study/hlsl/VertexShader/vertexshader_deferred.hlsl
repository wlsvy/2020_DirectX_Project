////////////////////////////////////////////////////////////////////////////////
// Filename: deferred.vs
////////////////////////////////////////////////////////////////////////////////


//////////////////////
// CONSTANT BUFFERS //
//////////////////////
cbuffer perObjectBuffer : register(b0)
{
    float4x4 wvpMatrix;
    float4x4 worldMatrix;
    float4x4 projectionMat;
};

//////////////
// TYPEDEFS //
//////////////
struct VS_INPUT
{
    float3 inPos : POSITION;
    float2 inTexCoord : TEXCOORD;
    float3 inNormal : NORMAL;
};

struct VS_OUTPUT
{
    float4 outPosition : SV_POSITION;
    float2 outTexCoord : TEXCOORD;
    float3 outNormal : NORMAL;
};

VS_OUTPUT main(VS_INPUT input)
{
    VS_OUTPUT output;
    output.outPosition = float4(input.inPos, 1.0f);
    output.outTexCoord = input.inTexCoord;

    return output;
}
