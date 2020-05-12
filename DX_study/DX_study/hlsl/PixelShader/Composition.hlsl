#include "Include/Common.hlsli"
#include "Include/PostProcessHeader.hlsli"
#include "Include/PBR.hlsli"
#include "Include/VolumetricLight.hlsli"
#include "Include/AmbientOcclusion.hlsli"


struct PS_INPUT
{
    float4 inPosition : SV_POSITION;
    float2 inTexCoord : TEXCOORD;
};

float4 main(PS_INPUT input) : SV_TARGET
{
    float4 position = positionTexture.Sample(PointClamp, input.inTexCoord);
    float3 normal = normalTexture.Sample(PointClamp, input.inTexCoord);
    float3 albedo = colorTexture.Sample(PointClamp, input.inTexCoord);
    float3 matProperty = matPropertyTexture.Sample(PointClamp, input.inTexCoord);
    float depth = depthTexture.Sample(PointClamp, input.inTexCoord).x;
    float metal = matProperty.x;
    float roughness = matProperty.y;
    float specular = matProperty.z;
    float ambientOcclusion = ssaoTexture.Sample(PointClamp, input.inTexCoord);
    
    float3 volumetricLight;
    float shadow;
    ProcessVl_ShadowTexure(input.inTexCoord, float2(1024, 768), depth, volumetricLight, shadow);
    
    float3 lightVal = AmbientColor * AmbientStrength * ambientOcclusion;
    lightVal += CalculateLightColor(position.xyz, normal.xyz) * shadow;
    float3 finalColor = float3(0.0f, 0.0f, 0.0f);
    
    float3 rayDir = normalize(position.xyz - CameraPosition.xyz);
    float3 reflectionVector = reflect(rayDir, normal);
    float3 reflectionColor = irradianceTexture.Sample(PointClamp, reflectionVector) * 0.5;
    
    if (position.w < 0.0f)
    {
        return float4(albedo + volumetricLight, 1.0f);
    }
       
    finalColor = volumetricLight + saturate(lightVal * albedo + reflectionColor);
    return float4(finalColor, 1.0f);
}