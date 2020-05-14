#include "Include/Common.hlsli"
#include "Include/Shadow.hlsli"
#include "Include/PBR.hlsli"
#include "Include/VolumetricLight.hlsli"

float4 main(Vertex_Quad input) : SV_TARGET
{
    float4 position = positionTexture.Sample(PointClamp, input.inTexCoord);
    float3 normal = normalTexture.Sample(PointClamp, input.inTexCoord).xyz;
    float3 albedo = colorTexture.Sample(PointClamp, input.inTexCoord).xyz;
    float3 matProperty = matPropertyTexture.Sample(PointClamp, input.inTexCoord).xyz;
    float depth = depthTexture.Sample(PointClamp, input.inTexCoord).x;
    float metal = matProperty.x;
    float roughness = matProperty.y;
    float specular = matProperty.z;
    float ambientOcclusion = ssaoTexture.Sample(PointClamp, input.inTexCoord).x;
    
    float3 rayDir = normalize(position.xyz - CameraPosition.xyz);
    float cameraToPixelDistance = length(position.xyz - CameraPosition.xyz);
    float3 vl = VolumetricLight(input.inTexCoord, position.xyz, rayDir, cameraToPixelDistance);

    
    if (position.w < 0.0f)
    {
        return float4(albedo + vl, 1.0f);
    }
    
    
    float4 lightSpacePos = mul(float4(position.xyz, 1.0f), transpose(spotLight.ViewProjMatrix));
    half2 shadow = CalculateShadowPCF(0, lightSpacePos);
    
    float3 LO = ComputeRadiance(normal, rayDir, position.xyz, albedo, metal, roughness);
    float3 ambient = ComputeAmbient(normal, rayDir, albedo, metal, roughness) * ambientOcclusion;
    
    return float4(LO * shadow.x + ambient + vl, 1.0f);
}