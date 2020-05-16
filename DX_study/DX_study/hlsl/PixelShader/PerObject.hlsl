#include "Include/Common.hlsli"

Pixel_Deferred main(Vertex_Basic input) : SV_TARGET
{
    Pixel_Deferred output;
    output.pos = float4(input.inWorldPos, 1.0f);
    output.color = albedoMap.Sample(PointClamp, input.inTexCoord) * materialColor;
    
    float3 calNormal = CalcPerPixelNormal(input.inTexCoord, input.inNormal, input.inTangent.xyz);
    output.normal = float4(calNormal, 1.0f);
    
    output.materialProperty = float4
    (
        metalMap.Sample(PointClamp, input.inTexCoord).r * MetalIntensity,
        roughnessMap.Sample(PointClamp, input.inTexCoord).r * RoughnessIntensity,
        specularMap.Sample(PointClamp, input.inTexCoord).r * SpecularIntensity,
        1.0f
    );
    
    float depth = input.inPosition.z / input.inPosition.w;
    output.depth = float4(depth, depth, depth, 1.0f);
    return output;
}