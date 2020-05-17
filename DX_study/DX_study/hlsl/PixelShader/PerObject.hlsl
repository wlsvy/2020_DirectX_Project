#include "Include/Common.hlsli"

Pixel_Deferred main(Vertex_Basic input) : SV_TARGET
{
    float3 normal = CalcPerPixelNormal(input.inTexCoord, input.inNormal, input.inTangent.xyz);
    
    Pixel_DeferredOpaque output;
    output.pos = input.inWorldPos;
    output.colorFlag = 1.0f;
    output.normal = normal;
    output.depth = input.inPosition.z / input.inPosition.w;
    output.color = albedoMap.Sample(PointClamp, input.inTexCoord).rgb * materialColor.rgb;
    output.metal = metalMap.Sample(PointClamp, input.inTexCoord).r * MetalIntensity;
    output.specular = specularMap.Sample(PointClamp, input.inTexCoord).rgb * SpecularIntensity;
    output.roughness = roughnessMap.Sample(PointClamp, input.inTexCoord).r * RoughnessIntensity;
    return output;
}