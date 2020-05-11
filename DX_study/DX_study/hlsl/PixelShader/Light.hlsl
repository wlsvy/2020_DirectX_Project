#include "Include/Common.hlsli"
#include "Include/PostProcessHeader.hlsli"
#include "Include/Shadow.hlsli"
#include "Include/VolumetricLight.hlsli"

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
    
    float4 lightSpacePos = mul(float4(position.xyz, 1.0f), transpose(spotLight.ViewProjMatrix));
    float4 output = float4
    (
        VolumetricLight(input.inTexCoord, position.xyz, rayDir, cameraToPixelDistance).xyz,
        CalculateShadow(0, lightSpacePos).x
    );
    
    return output;
}