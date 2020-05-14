#include "Include/Common.hlsli"
#include "Include/Shadow.hlsli"
#include "Include/PBR.hlsli"

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
    
    if (position.w < 0.0f)
    {
        return float4(albedo, 1.0f);
    }
    
    float3 rayDir = normalize(position.xyz - CameraPosition.xyz);
    
    float3 pbrColor = PBRmain(normal, rayDir, position.xyz, albedo, metal, roughness, ambientOcclusion).xyz;
    //pbrColor += AmbientColor * AmbientStrength * ambientOcclusion;
    //Gamma Correction
    //pbrColor = pbrColor / (pbrColor + 1.0);
    //pbrColor = pow(pbrColor, (1.0 / 2.2).xxx);
    
    // Exposure tone mapping
    pbrColor = 1.0 - exp(-pbrColor * HdrExposure);
    // Gamma correction 
    pbrColor = pow(pbrColor, 1.0 / Gamma);
    
    
    return float4(pbrColor, 1.0f);
    
    
    float4 lightInfo = lightShadowTexture.Sample(LinearWrap, input.inTexCoord);
    //return lightInfo;
    
    float4 lightSpacePos = mul(float4(position.xyz, 1.0f), transpose(spotLight.ViewProjMatrix));
    half2 shadow = CalculateShadowPCF(0, lightSpacePos);
    float3 volumetricLight = lightInfo.x * spotLight.Color;
    
    float3 lightVal = AmbientColor * AmbientStrength * ambientOcclusion;
    lightVal += CalculateLightColor(position.xyz, normal.xyz).xyz * shadow.x;
    float3 finalColor = float3(0.0f, 0.0f, 0.0f);
    
    //float3 rayDir = normalize(position.xyz - CameraPosition.xyz);
    float3 reflectionVector = reflect(rayDir, normal);
    float3 reflectionColor = irradianceTexture.Sample(PointClamp, reflectionVector).xyz * 0.5;
    
    if (position.w < 0.0f)
    {
        return float4(albedo + volumetricLight, 1.0f);
    }
       
    finalColor = volumetricLight + saturate(lightVal * albedo + reflectionColor);
    return float4(finalColor, 1.0f);
}