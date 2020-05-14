#ifndef PIXELSHADER_PBR_HLSLI
#define PIXELSHADER_PBR_HLSLI

#include "Common.hlsli"

static const float3 Fdielectric = 0.04;

float gaSchlickGGX(float NdotV, float roughness)  // k is a remapping of roughness based on direct lighting or IBL lighting
{
    float r = roughness + 1.0f;
    float k = (r * r) / 8.0f;

    float nom = NdotV;
    float denom = NdotV * (1.0f - k) + k;

    return nom / denom;
}

float GeometrySmith(float3 normalVec, float3 viewDir, float3 lightDir, float k)
{
    float NdotV = max(dot(normalVec, viewDir), 0.0f);
    float NdotL = max(dot(normalVec, lightDir), 0.0f);
    float ggx1 = gaSchlickGGX(NdotV, k);
    float ggx2 = gaSchlickGGX(NdotL, k);

    return ggx1 * ggx2;
}






//위에 코드는 제대로 동작 안됨
//슈벌 아래는 내가 직접 공부하고 알아봐서 가져온 코드

float NormalDistributionGGX(float3 N, float3 H, float a)
{
    float a2 = a * a;//dyrl
    float NdotH = max(dot(N, H), 0.0);
    float NdotH2 = NdotH * NdotH;
	
    float nom = a2;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;
	
    return nom / denom;
}

float GeometrySchlickGGX(float NdotV, float k)
{
    float nom = NdotV;
    float denom = NdotV * (1.0 - k) + k;
	
    return nom / denom;
}

float3 fresnelSchlick(float cosTheta, float3 F0)
{
    return F0 + (1.0 - F0) * pow(1.0 - cosTheta, 5.0);
}

float3 fresnelSchlick(float cosTheta, float3 F0, float roughness)
{
    return F0 + (max((1.0 - roughness).xxx, F0) - F0) * pow(1.0 - cosTheta, 5.0);
}

float4 PBRmain(float3 normal, float3 viewDir, float3 worldPos, float3 albedo, float metalic, float roughness, float ao)
{
    float3 N = normal;
    float3 V = -viewDir;
    float3 R = reflect(-V, N);

    float3 F0 = float3(0.04, 0.04, 0.04);
    F0 = lerp(F0, albedo, metalic);
	           
    // reflectance equation
    float3 Lo = float3(0.0, 0.0, 0.0);
    //for (int i = 0; i < 4; ++i)
    //{
    //    // calculate per-light radiance
    //    float3 L = normalize(lightPositions[i] - WorldPos);
    //    float3 H = normalize(V + L);
    //    float distance = length(lightPositions[i] - WorldPos);
    //    float attenuation = 1.0 / (distance * distance);
    //    float3 radiance = lightColors[i] * attenuation;
        
    //    // cook-torrance brdf
    //    float NDF = NormalDistributionGGX(N, H, roughness);
    //    float G = GeometrySmith(N, V, L, roughness);
    //    float3 F = fresnelSchlick(max(dot(H, V), 0.0), F0);
        
    //    float3 kS = F;
    //    float3 kD = float3(1.0) - kS;
    //    kD *= 1.0 - metalic;
        
    //    float3 numerator = NDF * G * F;
    //    float denominator = 4.0 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0);
    //    float3 specular = numerator / max(denominator, 0.001);
            
    //    // add to outgoing radiance Lo
    //    float NdotL = max(dot(N, L), 0.0);
    //    Lo += (kD * albedo / PI + specular) * radiance * NdotL;
    //}
    
    float3 L = normalize(spotLight.Position - worldPos);
    float3 H = normalize(V + L);
    float distance = length(spotLight.Position - worldPos);
    float attenuation = 100.0 / (distance * distance);
    float3 radiance = spotLight.Color * attenuation;
        
        // cook-torrance brdf
    float NDF = NormalDistributionGGX(N, H, roughness);
    float G = GeometrySmith(N, V, L, roughness);
    float3 F = fresnelSchlick(max(dot(H, V), 0.0), F0);
        
    float3 kS = F;
    float3 kD = 1.0 - kS;
    kD *= 1.0 - metalic;
        
    float3 numerator = NDF * G * F;
    float denominator = 4.0 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0);
    float3 specular = numerator / max(denominator, 0.001);
            
        // add to outgoing radiance Lo
    float NdotL = max(dot(N, L), 0.0);
    Lo += (kD * albedo / PI + specular) * radiance * NdotL;
  
    
    float3 irkS = fresnelSchlick(max(dot(N, V), 0.0), F0, roughness);
    float3 irkD = 1.0 - irkS;
    irkD *= 1.0 - metalic;
    float3 irradiance = irradianceTexture.Sample(LinearWrap, N).rgb + 0.5;
    float3 diffuse = irradiance * albedo;
    
    float3 prefilteredColor = skyBoxCube.SampleLevel(TrilinearWrap, R, roughness * 10).rgb;
    float2 envBRDF = specularBRDF_LUT.Sample(LinearMirror, float2(max(dot(N, V), 0.0), roughness)).rg;
    float3 irSpecular = prefilteredColor * (F * envBRDF.x + envBRDF.y);
    float3 ambient = (kD * diffuse + irSpecular) * ao;

    return float4(Lo +ambient, 1.0);
}
#endif