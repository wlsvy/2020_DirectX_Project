#include "Include/Common.hlsli"

float4 main(Vertex_Quad input) : SV_TARGET
{
    float4 position = positionTexture.Sample(PointClamp, input.inTexCoord);
    float3 normal = normalTexture.Sample(PointClamp, input.inTexCoord);
    float3 albedo = colorTexture.Sample(PointClamp, input.inTexCoord);
    float3 matProperty = matPropertyTexture.Sample(PointClamp, input.inTexCoord);
    float metal = matProperty.x;
    float roughness = matProperty.y;
    float specular = matProperty.z;
    
    float3 lightVal = AmbientColor * AmbientStrength;
    float3 finalColor = float3(0.0f, 0.0f, 0.0f);
    return float4(finalColor, 1.0f);
}