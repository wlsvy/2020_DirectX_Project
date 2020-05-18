
struct VS_DATA
{
    float3 inPos : POSITION;
    float2 inTexCoord : TEXCOORD;
    float3 inNormal : NORMAL;
    float4 inTangent : TANGENT;
};

VS_DATA main(VS_DATA input)
{
    return input;
}