
struct PS_INPUT
{
    float4 inPosition : SV_POSITION0;
    float2 inTexCoord : TEXCOORD0;
    float3 inNormal : NORMAL0;
    float3 inWorldPos : WORLD_POSITION0;
    float4 inLightPosition : TEXCOORD1;
    float4 inToLightDir : TEXCOORD2;
};


float4 main(PS_INPUT input) : SV_TARGET
{
    float depth = input.inPosition.z / input.inPosition.w;
    //float4 output = float4(1.0f, 0.0f, 1.0f, 1.0f);
    float4 output = float4(depth, depth, depth, 1.0f);
    
    return output;
}