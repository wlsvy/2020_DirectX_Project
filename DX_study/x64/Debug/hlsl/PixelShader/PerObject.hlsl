#include "Include/Common.hlsli"

Pixel_Deferred main(Vertex_Basic input) : SV_TARGET
{
    float3 normal = CalcPerPixelNormal(input.inTexCoord, input.inNormal, input.inTangent.xyz);
    
    Pixel_DeferredOpaque output;
    output.pos = input.inWorldPos;
    output.colorFlag = 1.0f;
    output.normal = normal;
    output.depth = input.inPosition.z / input.inPosition.w;
    output.color = MaterialAlbedoMap.Sample(PointClamp, input.inTexCoord).rgb * AlbedoColor.rgb;
    output.metal = MaterialMetalMap.Sample(PointClamp, input.inTexCoord).r * MetalIntensity;
    output.specular = MaterialSpecularMap.Sample(PointClamp, input.inTexCoord).rgb * SpecularIntensity;
    output.roughness = MaterialRoughMap.Sample(PointClamp, input.inTexCoord).r * RoughnessIntensity;
    return output;
}