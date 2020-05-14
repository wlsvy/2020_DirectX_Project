#include "Include/Common.hlsli"
#include "Include/Shadow.hlsli"
#include "Include/VolumetricLight.hlsli"

float4 main(Vertex_Quad input) : SV_TARGET
{
    float4 position = positionTexture.Sample(PointClamp, input.inTexCoord);
    float3 normal = normalTexture.Sample(PointClamp, input.inTexCoord).xyz;
    float3 albedo = colorTexture.Sample(PointClamp, input.inTexCoord).xyz;
    float3 matProperty = matPropertyTexture.Sample(PointClamp, input.inTexCoord).xyz;
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
        VolumetricLight(input.inTexCoord, position.xyz, rayDir, cameraToPixelDistance).x,
        lightSpacePos.xy / lightSpacePos.w,
        //lightSpacePos.z * 0.02f,
        1.0f
    );
    
    return output;
}