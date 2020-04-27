#include "pixelshader_ObjectCommon.hlsli"

float4 main(PS_INPUT input) : SV_TARGET
{
    float4 newPos = mul(float4(input.inWorldPos, 1.0f), transpose(spotLight.ViewProjMatrix));
    float depth = 1 - newPos.z / spotLight.Range;
    //float depth = input.inPosition.z / input.inPosition.w;

    float4 output = float4(depth, depth, depth, 1.0f);
    
    return output;
}