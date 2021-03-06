#ifndef PIXELSHADER_PBR_HLSLI
#define PIXELSHADER_PBR_HLSLI

#include "Common.hlsli"

float gaSchlickGGX(float NdotV, float roughness)
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

float NormalDistributionGGX(float3 N, float3 H, float a)
{
    float a2 = a * a;
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

float3 ComputeRadiance(float3 normal, float3 viewDir, float3 worldPos, float3 albedo, float metalic, float roughness)
{
    float3 N = normal;
    float3 V = -viewDir;

    float3 F0 = float3(0.04, 0.04, 0.04);
    F0 = lerp(F0, albedo, metalic);
	           
    float3 L = normalize(spotLight.Position - worldPos);
    float3 H = normalize(V + L);
    float distance = length(spotLight.Position - worldPos);
    float3 radiance = ComputeSpotLightColor(L, N, distance);;
        
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
    return (kD * albedo / PI + specular) * radiance * NdotL;
}

float3 ComputeAmbient(float3 normal, float3 viewDir, float3 albedo, float metalic, float roughness)
{
    float3 N = normal;
    float3 V = -viewDir;

    float3 F0 = float3(0.04, 0.04, 0.04);
    F0 = lerp(F0, albedo, metalic);
    
    float3 kS = fresnelSchlick(max(dot(N, V), 0.0), F0, roughness);
    float3 kD = 1.0 - kS;
    kD *= 1.0 - metalic;
    float3 irradiance = IrradianceCubeMap.Sample(LinearWrap, N).rgb * SkyBoxColor;
    float3 diffuse = irradiance * AmbientColor * AmbientStrength * albedo;
    
    float3 R = reflect(-V, N);
    
    float3 prefilteredColor = SkyboxCubeMap.SampleLevel(TrilinearWrap, R, roughness * Skybox_MaxMipMapLevel).rgb * SkyBoxColor;
    float2 envBRDF = BRDF_LUT_IBL.Sample(LinearMirror, float2(max(dot(N, V), 0.0), roughness)).rg;
    float3 irSpecular = prefilteredColor * (kS * envBRDF.x + envBRDF.y);
    float3 ambient = (kD * diffuse + irSpecular);

    return ambient;
}

#endif