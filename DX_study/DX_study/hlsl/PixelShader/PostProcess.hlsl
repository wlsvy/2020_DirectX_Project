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
    float metal = matProperty.x;
    float roughness = matProperty.y;
    float specular = matProperty.z;
    
    float3 lightVal = AmbientColor * AmbientStrength;
    float3 finalColor = float3(0.0f, 0.0f, 0.0f);
    
    float3 rayDir = normalize(position.xyz - CameraPosition.xyz);
    float cameraToPixelDistance = length(position.xyz - CameraPosition.xyz);
    cameraToPixelDistance = min(cameraToPixelDistance, 1000.0f);

    float4 lightSpacePos = mul(float4(position.xyz, 1.0f), transpose(spotLight.ViewProjMatrix)); // 도대체 왜 이걸 전치시켜야 하는지 이해를 못하겠다
    //lightVal += CalculateLightColor(position.xyz, normal.xyz);
    lightVal += CalculateShadow(0, lightSpacePos) * CalculateLightColor(position.xyz, normal.xyz);
    
    //float3 reflectionVector = reflect(rayDir, normal);
    //float3 reflectionColor = skyBoxCube.Sample(PointClamp, reflectionVector) * 0.5;
    
    float ambientOcclusionFactor = ComputeAmbientOcclusion(position.xyz, normal.xyz, input.inTexCoord);
    
    float4 vl = VolumetricLight(input.inTexCoord, position.xyz, rayDir, cameraToPixelDistance);
    
    if (position.w < 0.0f)
        return float4(albedo + vl.xyz, 1.0f);
    
    finalColor = vl.xyz + saturate(lightVal * albedo);
    return float4(finalColor, 1.0f);
}