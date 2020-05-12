#ifndef PIXELSHADER_PBR_HLSLI
#define PIXELSHADER_PBR_HLSLI

#include "Common.hlsli"
#include "PostProcessHeader.hlsli"

static const float3 Fdielectric = 0.04;

// GGX/Towbridge-Reitz normal distribution function.
// Uses Disney's reparametrization of alpha = roughness^2.
float ndfGGX(float cosLh, float roughness)
{
    float alpha = roughness * roughness;
    float alphaSq = alpha * alpha;

    float denom = (cosLh * cosLh) * (alphaSq - 1.0) + 1.0;
    return alphaSq / (PI * denom * denom);
}
float ndfGGX(float3 normalVec, float3 halfwayVec, float roughness)
{
    float a = roughness * roughness;
    float a2 = a * a; // a2 = a^2
    float NdotH = max(dot(normalVec, halfwayVec), 0.0); // NdotH = normalVec.halfwayVec
    float NdotH2 = NdotH * NdotH; // NdotH2 = NdotH^2
    float nom = a2;
    float denom = (NdotH2 * (a2 - 1.0f) + 1.0f);
    denom = PI * denom * denom;

    return nom / denom;
}


// Single term for separable Schlick-GGX below.
float gaSchlickG1(float cosTheta, float k)
{
    return cosTheta / (cosTheta * (1.0 - k) + k);
}

// Schlick-GGX approximation of geometric attenuation function using Smith's method.
float gaSchlickGGX(float cosLi, float cosLo, float roughness)
{
    float r = roughness + 1.0;
    float k = (r * r) / 8.0; // Epic suggests using this roughness remapping for analytic lights.
    return gaSchlickG1(cosLi, k) * gaSchlickG1(cosLo, k);
}

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

// Shlick's approximation of the Fresnel factor.
float3 fresnelSchlick(float3 F0, float cosTheta)
{
    return F0 + (1.0 - F0) * pow(1.0 - cosTheta, 5.0);
}

float3 fresnelSchlick(float cosTheta, float3 F0, float roughness)   // cosTheta is n.v and F0 is the base reflectivity
{
    return F0 + (max(float3(1.0f - roughness, 1.0f - roughness, 1.0f - roughness), F0) - F0) * pow(1.0 - cosTheta, 5.0f);
}


float3 PhysicsBasedLighting(float3 position, float3 normal, float3 albedo, float metal, float specular, float roughness)
{
    float3 directLighting = 0.0;
    float3 ambientLighting;
    {
        float3 Lo = normalize(CameraPosition - position);

        float3 N = normalize(2.0 * normal - 1.0);
        //N = normalize(mul(pin.tangentBasis, N));
	
        float cosLo = max(0.0, dot(N, Lo));
        float3 Lr = 2.0 * cosLo * N - Lo;

        float3 F0 = lerp(Fdielectric, albedo, metal);
        //float3 directLighting = 0.0;
        
        //per Light
        {
            float3 Li = -spotLight.Forward;
            float3 Lradiance = float3(1.0f, 1.0f, 1.0f);

            float3 Lh = normalize(Li + Lo);
            float cosLi = max(0.0, dot(N, Li));
            float cosLh = max(0.0, dot(N, Lh));
            float3 F = fresnelSchlick(F0, max(0.0, dot(Lh, Lo)));
            float D = ndfGGX(cosLh, roughness);
            float G = gaSchlickGGX(cosLi, cosLo, roughness);

            float3 kd = lerp(float3(1, 1, 1) - F, float3(0, 0, 0), metal);
            float3 diffuseBRDF = kd * albedo;
            float3 specularBRDF = (F * D * G) / max(Epsilon, 4.0 * cosLi * cosLo);
            directLighting += (diffuseBRDF + specularBRDF) * Lradiance * cosLi;
        }
        return directLighting;

        
        {
        
            float3 irradiance = irradianceTexture.Sample(PointClamp, N).rgb;
            float3 F = fresnelSchlick(F0, cosLo);

            float3 kd = lerp(1.0 - F, 0.0, metal);

            float3 diffuseIBL = kd * albedo * irradiance;

            //uint specularTextureLevels = querySpecularTextureLevels();
            //float3 specularIrradiance = specularTexture.SampleLevel(defaultSampler, Lr, roughness * specularTextureLevels).rgb;

            float2 specularBRDF = specularBRDF_LUT.Sample(PointClamp, float2(cosLo, roughness)).rg;

            float3 specularIBL = (F0 * specularBRDF.x + specularBRDF.y) * specular;

            ambientLighting = diffuseIBL + specularIBL;
        }
    }
    
    return directLighting + ambientLighting;
}


void CalcRadiance(float3 lightDir, float3 viewDir, float3 normalVec, float3 albedo, float roughness, float metallic, float3 lightPos, float3 lightCol, float3 F0, out float3 rad)
{
    static const float PI = 3.14159265359;

	//calculate light radiance
    float3 halfwayVec = normalize(viewDir + lightDir);
    float3 radiance = lightCol;

	//Cook-Torrance BRDF
    float D = ndfGGX(normalVec, halfwayVec, roughness);
    float G = GeometrySmith(normalVec, viewDir, lightDir, roughness);
    float3 F = fresnelSchlick(max(dot(halfwayVec, viewDir), 0.0f), F0.r);

    float3 kS = F;
    float3 kD = float3(1.0f, 1.0f, 1.0f) - kS;
    kD *= 1.0 - metallic;

    float3 nom = D * G * F;
    float denom = 4 * max(dot(normalVec, viewDir), 0.0f) * max(dot(normalVec, lightDir), 0.0) + 0.001f; // 0.001f just in case product is 0
    float3 specular = nom / denom;

	//Add to outgoing radiance Lo
    float NdotL = max(dot(normalVec, lightDir), 0.0f);
    rad = (((kD * albedo / PI) + specular) * radiance * NdotL);
}

//float4 Pbr2(float3 wpos, float3 albedo, float3 normalVec, float metallic, float rough) : SV_TARGET
//{
//    float3 albedo = pow(albedo, 2.2f);
	
//    //float3 normalFromMap = normalSRV.Sample(basicSampler, input.uv).xyz * 2 - 1;
//    //float3 normalVec = input.normal;
	
//    float3 viewDir = normalize(CameraPosition - wpos);
	
//    float3 R = reflect(-viewDir, normalVec);

//    float3 F0 = float3(0.04f, 0.04f, 0.04f);
//    F0 = lerp(F0, albedo, metallic);

//    float3 rad = float3(0.0f, 0.0f, 0.0f);
//	//reflectance equation
//    float3 Lo = float3(0.0f, 0.0f, 0.0f);
    
//    float3 lightDir = spotLight.Position - wpos;
//    float lightDist = length(lightDir);
//    lightDir /= lightDist;

//    CalcRadiance(lightDir, viewDir, normalVec, albedo, rough, metallic, spotLight.Position, spotLight.Color / pow(lightDist, 2), F0, rad);
//    Lo += rad;

//    float3 kS = fresnelSchlick(max(dot(normalVec, viewDir), 0.0f), F0, rough);
//    float3 kD = float3(1.0f, 1.0f, 1.0f) - kS;
//    kD *= 1.0 - metallic;

//    float3 irradiance = irradianceTexture.Sample(PointClamp, normalVec).rgb;
//    float3 diffuse = albedo * irradiance;

//    const float MAX_REF_LOD = 4.0f;
//    float3 prefilteredColor = irradianceTexture.SampleLevel(PointClamp, R, rough * MAX_REF_LOD).rgb;
//    float2 brdf = brdfLUT.Sample(basicSampler, float2(max(dot(normalVec, viewDir), 0.0f), rough)).rg;
//    float3 specular = prefilteredColor * (kS * brdf.x + brdf.y);

//    float3 ambient = (kD * diffuse + specular) * ao;
//    float3 color = ambient + Lo;


//    color = color / (color + float3(1.0f, 1.0f, 1.0f));
//    color = pow(color, float3(1.0f / 2.2f, 1.0f / 2.2f, 1.0f / 2.2f));

//    return float4(color, 1.0f);
//}
#endif