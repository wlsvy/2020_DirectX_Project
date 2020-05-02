#include "../pixelshader_Common.hlsli"

Texture2D positionTexture : register(t0);
Texture2D normalTexture : register(t1);
Texture2D colorTexture : register(t2);
Texture2D matPropertyTexture : register(t3);
Texture2D depthTexture : register(t4);
Texture2D shadowMap :  register(t5);

Texture2D randomMap : register(t8);
TextureCube skyBoxCube : register(t9);
Texture2D DitheringTexture : register(t10);

struct PS_INPUT
{
    float4 inPosition : SV_POSITION;
    float2 inTexCoord : TEXCOORD;
};

float4 CalculateShadow(int lightIndex, float4 lightSpacePos)
{
    float lightDepth = 1 - lightSpacePos.z / spotLight.Range + 0.0001;
    
    float2 projectTexCoord = 0.5f * lightSpacePos.xy / lightSpacePos.w + 0.5f;
    projectTexCoord.y = 1.0f - projectTexCoord.y;
        
    if (projectTexCoord.x <= 0.0f || projectTexCoord.x >= 1.0f || projectTexCoord.y <= 0.0f || projectTexCoord.y >= 1.0f || lightDepth > 1.0f)
    {
        return float4(1.0f, 1.0f, 1.0f, 1.0f);
    }
    
    float shadowMapDepth = shadowMap.Sample(LinearWrap, projectTexCoord).r;
    
    if (lightDepth >= shadowMapDepth)
    {
        return float4(1.0f, 1.0f, 1.0f, 1.0f);
    }
    return float4(0.0f, 0.0f, 0.0f, 0.0f);
}

float FogAttenuation(float density, float distance)
{
    return saturate(1 / exp(distance * density));
}

float2 getRandom(in float2 uv)
{
    return normalize(randomMap.Sample(LinearWrap, uv).xy * 2.0f - 1.0f);
}

//출처 : https://www.gamedev.net/articles/programming/graphics/a-simple-and-practical-approach-to-ssao-r2753/
float ComputeOcclusionPixel(in float2 tcoord, in float2 uv, in float3 p, in float3 cnorm)
{
    float3 diff = positionTexture.Sample(PointClamp, tcoord + uv).xyz - p;
    const float3 v = normalize(diff);
    const float d = length(diff) * SSAO_scale;
    return max(0.0, dot(cnorm, v) - SSAO_bias) * (1.0 / (1.0 + d)) * SSAO_strength;
}

const float SSf = 0.7;
#define SSAO_FACTOR 0.3
float ComputeAmbientOcclusion(float3 position, float3 normal, float2 texcoord)
{
    const float2 vec[4] = { float2(1, 0), float2(-1, 0), float2(0, 1), float2(0, -1) };
    float3 p = position;
    float3 n = normal;
    //float2 rand = float2(1, 1);
    float2 rand = getRandom(texcoord);
    float ao = 0.0f;
    float rad = SSAO_radius / p.z;
  
  //**SSAO Calculation**// 
    int iterations = 4; 
    for (int i = 0; i < iterations; ++i)
    {
        float2 coord1 = reflect(vec[i], rand) * rad;
        float2 coord2 = float2(coord1.x * SSAO_FACTOR - coord1.y * SSAO_FACTOR, coord1.x * SSAO_FACTOR + coord1.y * SSAO_FACTOR);
    
        ao += ComputeOcclusionPixel(texcoord, coord1 * 0.25, p, n);
        ao += ComputeOcclusionPixel(texcoord, coord2 * 0.5, p, n);
        ao += ComputeOcclusionPixel(texcoord, coord1 * 0.75, p, n);
        ao += ComputeOcclusionPixel(texcoord, coord2, p, n);
    }
  
    return 1 - saturate(ao / (float) iterations * 4.0);
}


//Volumetric Light  참조 : https://github.com/SlightlyMad/VolumetricLights, https://github.com/ACskyline/VLD3D11
bool DoesLightReach(float3 position, float3 lightVec, float lightDist)
{
    if (!ContainedBySpotLight(lightVec, lightDist))
    {
        return false;
    }
    
    float4 lightSpacePos = mul(float4(position, 1.0f), transpose(spotLight.ViewProjMatrix)); // 도대체 왜 이걸 전치시켜야 하는지 이해를 못하겠다
    if (lightSpacePos.w < 0)
        return false;
    
    return CalculateShadow(0, lightSpacePos);;
}


float4 RayMarch(float2 screenPos, float3 rayStart, float3 rayDir, float rayLength)
{
    float2 interleavedPos = (fmod(floor(screenPos.xy), 8.0));
    float offset = DitheringTexture.Sample(LinearWrap, interleavedPos / 8.0 + float2(0.5 / 8.0, 0.5 / 8.0)).x;
    int stepCount = VolumetricLightSampleCount;

    float stepSize = rayLength / stepCount;
    float3 step = rayDir * stepSize;

    float3 currentPosition = rayStart + step * offset;
    float4 vlight = 0;

    [loop]
    for (int i = 0; i < stepCount; ++i)
    {
        float3 randomOffset = GetRandomVector(currentPosition.z) * 0.3;
        float3 vectorToLight = spotLight.Position - currentPosition;
        float distToLight = length(vectorToLight);
        vectorToLight /= distToLight;
        
        if (DoesLightReach(currentPosition, -vectorToLight, distToLight))
        {
            float atten = 1 / (spotLight.Attenuation.x + (spotLight.Attenuation.y * distToLight) + (spotLight.Attenuation.z * pow(distToLight, 2)));

            float scattering = VolumetricLightVar.x * stepSize;

            float4 light = atten * scattering;
            vlight += light;
        }
        
        currentPosition += step;
    }

    vlight = max(0, vlight);
    vlight.w = 0;
    return vlight;
}

float4 VolumetricLight(float2 uv, float3 wpos)
{
    float linearDepth = depthTexture.Sample(LinearWrap, uv);

    float3 rayStart = CameraPosition;
    float3 rayDir = wpos - CameraPosition;
   
    float rayLength = length(rayDir);
    rayDir /= rayLength;

    float3 rayEnd = wpos + rayDir * 0.001;
    
    float near, far;
    bool2 hit = RayConeIntersect(rayStart, rayDir, near, far);
    
    float diskRad = tan(radians(spotLight.SpotAngle * 0.5)) * (spotLight.Range * 1.2);

    float diskT;
    bool dResult = RayDiskIntersect(spotLight.Forward, spotLight.conePlaneD, diskRad, rayStart, rayDir, diskT);
    
    float4 color = float4(0.0f, 0.0f, 0.0f, 1.0f);
    
    if (hit.x || dResult)
    {
        float minRay = min(diskT, near);
        float maxRay = min(max(dResult * diskT, max(near * hit.x, far * hit.y)), rayLength);
    
        if (!ContainedBySpotLight(CameraPosition))
        {
            rayStart += rayDir * minRay;
            rayLength = maxRay - minRay;
        }
        else
        {
            rayLength = maxRay;
        }
        color = RayMarch(uv, rayStart, rayDir, rayLength);

    }
    
    color *= float4(spotLight.Color, 1.0f);
    return color;

}   

float4 main(PS_INPUT input) : SV_TARGET
{    
    float4 texturePos = positionTexture.Sample(PointClamp, input.inTexCoord);
    float3 textureNormal = normalTexture.Sample(PointClamp, input.inTexCoord);
    float3 textureColor = colorTexture.Sample(PointClamp, input.inTexCoord);
    
    float3 lightVal = AmbientColor * AmbientStrength;
    float3 finalColor = float3(0.0f, 0.0f, 0.0f);
    
    float3 rayDir = normalize(texturePos.xyz - CameraPosition.xyz);
    float cameraToPixelDistance = length(texturePos.xyz - CameraPosition.xyz);
    cameraToPixelDistance = min(cameraToPixelDistance, 1000.0f);

    float4 lightSpacePos = mul(float4(texturePos.xyz, 1.0f), transpose(spotLight.ViewProjMatrix)); // 도대체 왜 이걸 전치시켜야 하는지 이해를 못하겠다
    lightVal += CalculateShadow(0, lightSpacePos) * CalculateLightColor(texturePos.xyz, textureNormal.xyz);
    
    float3 reflectionVector = reflect(rayDir, textureNormal);
    float3 reflectionColor = skyBoxCube.Sample(PointClamp, reflectionVector) * 0.5;
    
    float ambientOcclusionFactor = ComputeAmbientOcclusion(texturePos.xyz, textureNormal.xyz, input.inTexCoord);
    
    float4 vl = VolumetricLight(input.inTexCoord, texturePos.xyz);
    
    if (texturePos.w < 0.0f)
        return float4(textureColor + vl, 1.0f);
    
    //finalColor = scatterLight + saturate(lightVal * textureColor * ambientOcclusionFactor + reflectionColor);
    //finalColor = max(vl.xyz, saturate(lightVal * textureColor + reflectionColor));
    finalColor = vl.xyz + saturate(lightVal * textureColor + reflectionColor);
    return float4(finalColor, 1.0f);
    //return float4(vl.xyz, 1.0f);
}