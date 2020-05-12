#ifndef PIXELSHADER_VOLUMETRICLIGHT_HLSLI_
#define PIXELSHADER_VOLUMETRICLIGHT_HLSLI_

#include "Common.hlsli"
#include "Shadow.hlsli"

//Volumetric Light  ÂüÁ¶ : https://github.com/SlightlyMad/VolumetricLights, https://github.com/ACskyline/VLD3D11
bool DoesLightReach(float3 position, float3 lightVec, float lightDist)
{
    if (!ContainedBySpotLight(lightVec, lightDist))
    {
        return false;
    }
    
    float4 lightSpacePos = mul(float4(position, 1.0f), transpose(spotLight.ViewProjMatrix));
    if (lightSpacePos.w < 0)
        return false;
    
    return CalculateShadow(0, lightSpacePos).r;
}

float RayMarch(float2 screenPos, float3 rayStart, float3 rayDir, float rayLength)
{
    float2 interleavedPos = (fmod(floor(float2(screenPos.x * 1024, screenPos.y * 768)), 16.0));
    float offset = DitheringTexture.Sample(LinearWrap, interleavedPos / 16.0 + float2(0.5 / 8.0, 0.5 / 8.0)).x;
    int stepCount = VolumetricLightSampleCount;

    float stepSize = rayLength / stepCount;
    float3 step = rayDir * stepSize;

    float3 currentPosition = rayStart + step * offset;
    float vlight = 0;

    [loop]
    for (int i = 0; i < stepCount; i++)
    {
        float3 randomOffset = GetRandomVector(currentPosition.z) * 0.3;
        float3 vectorToLight = spotLight.Position - currentPosition;
        float distToLight = length(vectorToLight);
        vectorToLight /= distToLight;
        
        if (DoesLightReach(currentPosition, -vectorToLight, distToLight))
        {
            float atten = 1 / (spotLight.Attenuation.x + (spotLight.Attenuation.y * distToLight) + (spotLight.Attenuation.z * pow(distToLight, 2)));

            float scattering = VolumetricLightVar.x * stepSize;

            float light = atten * scattering;
            vlight += light;
        }
        
        currentPosition += step;
    }

    vlight = max(0, vlight);
    return vlight;
}

float4 VolumetricLight(float2 uv, float3 wpos, float3 rayDir, float rayLength)
{
    float linearDepth = depthTexture.Sample(LinearWrap, uv).x;

    float3 rayStart = CameraPosition;
    float3 rayEnd = wpos + rayDir * 0.001;
    
    float near, far;
    bool2 hit = RayConeIntersect(rayStart, rayDir, near, far);
    
    float diskRad = tan(radians(spotLight.SpotAngle * 0.5)) * (spotLight.Range * 1.2);

    float diskT;
    bool diskHit = RayDiskIntersect(spotLight.Forward, spotLight.conePlaneD, diskRad, rayStart, rayDir, diskT);
    
    float4 color = float4(0.0f, 0.0f, 0.0f, 1.0f);
    float lightStength = 0.0f;
    
    if (hit.x || diskHit)
    {
        float minRay = min(diskT, near);
        float maxRay = min(max(diskHit * diskT, max(near * hit.x, far * hit.y)), rayLength);
    
        if (!ContainedBySpotLight(CameraPosition))
        {
            rayStart += rayDir * minRay;
            rayLength = maxRay - minRay;
        }
        else
        {
            rayLength = maxRay;
        }
        lightStength = RayMarch(uv, rayStart, rayDir, rayLength);

    }
    
    //lightStength = saturate(pow(lightStength * 0.5 + VolumetricLightVar.z, VolumetricLightVar.y));
    //color *= float4(spotLight.Color, 1.0f);
    return float4(lightStength.xxx, 1.0f) * float4(spotLight.Color, 1.0f);

}

#endif