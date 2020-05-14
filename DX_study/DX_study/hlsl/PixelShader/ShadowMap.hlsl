#include "Include/Common.hlsli"

float4 main(Vertex_Basic input) : SV_TARGET
{
    float4 newPos = mul(float4(input.inWorldPos, 1.0f), transpose(spotLight.ViewProjMatrix));
    float InverseLinearDepth = 1 - newPos.z * 0.02;
    //float depth = input.inPosition.z / input.inPosition.w;
    //float depth = newPos.z / newPos.w;

    float4 output = float4(InverseLinearDepth.xxx, 1.0f);
    
    return output;
}