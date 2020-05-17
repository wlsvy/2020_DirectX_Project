#include "Include/Common.hlsli"
#include "Include/Shadow.hlsli"
#include "Include/PBR.hlsli"
#include "Include/VolumetricLight.hlsli"

float4 main(Vertex_Quad input) : SV_TARGET
{
    float4 Tex0 = DeferredRenderingResource0.Sample(PointClamp, input.inTexCoord);
    float4 Tex1 = DeferredRenderingResource1.Sample(PointClamp, input.inTexCoord);
    float4 Tex2 = DeferredRenderingResource2.Sample(PointClamp, input.inTexCoord);
    float4 Tex3 = DeferredRenderingResource3.Sample(PointClamp, input.inTexCoord);

    float3 position = Tex0.xyz;
    float3 albedo = Tex1.xyz;
    float3 normal = Tex2.xyz;
    float3 matProperty = Tex3.xyz;
    float metal = matProperty.x;
    float roughness = matProperty.y;
    float specular = matProperty.z;
    float ambientOcclusion = ssaoTexture.Sample(PointClamp, input.inTexCoord).x;
    float depth = Tex0.w;
    float colorFlag = Tex1.w;
    //return float4(depth.yyy, 1.0f);
    return float4(ambientOcclusion.xxx, 1.0f);
    
    float3 rayDir = normalize(position.xyz - CameraPosition.xyz);
    float cameraToPixelDistance = length(position.xyz - CameraPosition.xyz);
    float3 vl = VolumetricLight(input.inTexCoord, position.xyz, rayDir, cameraToPixelDistance);

    
    if (colorFlag < 0.0f)
    {
        return float4(albedo + vl, 1.0f);
    }
    
    
    float4 lightSpacePos = mul(float4(position.xyz, 1.0f), transpose(spotLight.ViewProjMatrix));
    half2 shadow = CalculateShadowPCF(0, lightSpacePos);
    
    float3 LO = ComputeRadiance(normal, rayDir, position.xyz, albedo, metal, roughness);
    float3 ambient = ComputeAmbient(normal, rayDir, albedo, metal, roughness) * ambientOcclusion;
    
    return float4(LO * shadow.x + ambient + vl, 1.0f);
}