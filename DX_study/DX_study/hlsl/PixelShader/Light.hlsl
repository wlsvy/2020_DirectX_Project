#include "Include/Common.hlsli"
#include "Include/Shadow.hlsli"
#include "Include/VolumetricLight.hlsli"
#include "Include/PBR.hlsli"

float4 main(Vertex_Quad input) : SV_TARGET
{
    float4 position = DeferredRenderingResource0.Sample(PointClamp, input.inTexCoord);
    float3 normal = DeferredRenderingResource1.Sample(PointClamp, input.inTexCoord).xyz;
    float3 albedo = DeferredRenderingResource2.Sample(PointClamp, input.inTexCoord).xyz;
    float3 matProperty = DeferredRenderingResource3.Sample(PointClamp, input.inTexCoord).xyz;
    float metal = matProperty.x;
    float roughness = matProperty.y;
    float specular = matProperty.z;
    float ambientOcclusion = ssaoTexture.Sample(PointClamp, input.inTexCoord).x;
    
    float3 lightVal = AmbientColor * AmbientStrength;
    float3 finalColor = float3(0.0f, 0.0f, 0.0f);
    
    float3 rayDir = normalize(position.xyz - CameraPosition.xyz);
    float cameraToPixelDistance = length(position.xyz - CameraPosition.xyz);
    cameraToPixelDistance = min(cameraToPixelDistance, 1000.0f);
    
    //float4 lightSpacePos = mul(float4(position.xyz, 1.0f), transpose(spotLight.ViewProjMatrix));
    //float4 output = float4
    //(
    //    VolumetricLight(input.inTexCoord, position.xyz, rayDir, cameraToPixelDistance).x,
    //    lightSpacePos.xy / lightSpacePos.w,
    //    //lightSpacePos.z * 0.02f,
    //    1.0f
    //);
    
    float4 output = 0.0f;
    
    return output;
}