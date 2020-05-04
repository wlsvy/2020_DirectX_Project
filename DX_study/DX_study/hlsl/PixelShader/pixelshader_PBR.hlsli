#ifndef PIXELSHADER_PBR_HLSLI
#define PIXELSHADER_PBR_HLSLI

#include "pixelshader_Common.hlsli"
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

// Shlick's approximation of the Fresnel factor.
float3 fresnelSchlick(float3 F0, float cosTheta)
{
    return F0 + (1.0 - F0) * pow(1.0 - cosTheta, 5.0);
}

//float3 PhysicsBasedLighting()
//{
//    float3 directLighting = 0.0;
//    float3 ambientLighting;
//    {
//        float3 Lo = normalize(CameraPosition - position.xyz);

//        float3 N = normalize(2.0 * normal.rgb - 1.0);
//        //N = normalize(mul(pin.tangentBasis, N));
	
//        float cosLo = max(0.0, dot(N, Lo));
//        float3 Lr = 2.0 * cosLo * N - Lo;

//        float3 F0 = lerp(Fdielectric, albedo, metal);
//        //float3 directLighting = 0.0;
        
//        //per Light
//        {
//            float3 Li = -spotLight.Forward;
//            float3 Lradiance = float3(1.0f, 1.0f, 1.0f);

//            float3 Lh = normalize(Li + Lo);
//            float cosLi = max(0.0, dot(N, Li));
//            float cosLh = max(0.0, dot(N, Lh));
//            float3 F = fresnelSchlick(F0, max(0.0, dot(Lh, Lo)));
//            float D = ndfGGX(cosLh, roughness);
//            float G = gaSchlickGGX(cosLi, cosLo, roughness);

//            float3 kd = lerp(float3(1, 1, 1) - F, float3(0, 0, 0), metal);
//            float3 diffuseBRDF = kd * albedo;
//            float3 specularBRDF = (F * D * G) / max(Epsilon, 4.0 * cosLi * cosLo);
//            directLighting += (diffuseBRDF + specularBRDF) * Lradiance * cosLi;
//        }
        

        
//        {
        
//            float3 irradiance = irradianceTexture.Sample(PointClamp, N).rgb;
//            float3 F = fresnelSchlick(F0, cosLo);

//            float3 kd = lerp(1.0 - F, 0.0, metal);

//            float3 diffuseIBL = kd * albedo * irradiance;

//            //uint specularTextureLevels = querySpecularTextureLevels();
//            //float3 specularIrradiance = specularTexture.SampleLevel(defaultSampler, Lr, roughness * specularTextureLevels).rgb;

//            float2 specularBRDF = specularBRDF_LUT.Sample(PointClamp, float2(cosLo, roughness)).rg;

//            float3 specularIBL = (F0 * specularBRDF.x + specularBRDF.y) * specular;

//            ambientLighting = diffuseIBL + specularIBL;
//        }
//    }
    
//    return directLighting + ambientLighting;
//}

#endif