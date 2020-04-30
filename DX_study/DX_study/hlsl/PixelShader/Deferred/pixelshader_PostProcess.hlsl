#include "../pixelshader_Common.hlsli"

Texture2D positionTexture : register(t0);
Texture2D normalTexture : register(t1);
Texture2D colorTexture : register(t2);
//Texture2D lightTexture : register(t3);
Texture2D shadowMap :  register(t3);
Texture2D depthTexture : register(t4);

Texture2D randomMap : register(t8);
TextureCube skyBoxCube : register(t9);

struct PS_INPUT
{
    float4 inPosition : SV_POSITION;
    float2 inTexCoord : TEXCOORD;
};

float InScatter(float3 rayStart, float3 rayDir, float3 lightPos, float dist)
{
    float scatterParamScale = 1.0f;
    float scatterProbability = 1.f / (4.f * PI); // normalization term for an isotropic phase function        
    float3 q = rayStart - lightPos; // light to ray origin
    float b = dot(rayDir, q);
    float c = dot(q, q);

    // Evaluate integral.
    float s = 1.0f / sqrt(max(c - b * b, 0.00001f));
    float l = s * (atan((dist + b) * s) - atan(b * s));
    l *= scatterParamScale * scatterProbability;

    return l;
}

float4 CalculateShadow(int lightIndex, float4 lightSpacePos)
{
    float lightDepth = 1 - lightSpacePos.z / spotLight.Range + 0.01;
    
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

float3 CalculateWorldPositionFromDepthMap(float2 screenCoord)
{
    // Translate from homogeneous coords to texture coords.
    float2 depthTexCoord = 0.5f * screenCoord + 0.5f;
    depthTexCoord.y = 1.0f - depthTexCoord.y;
    
    //float depth = depthTexture.Sample(PointClamp, depthTexCoord).r;
    float depth = depthTexture.Sample(PointClamp, depthTexCoord).w;

    float4 screenPos = float4(screenCoord.x, screenCoord.y, depth, 1.0);
    float4 viewPosition = mul(screenPos, InverseProjMatrix);
    viewPosition /= viewPosition.w; // Perspective division
    //float4 worldPosition = mul(viewPosition, viewInverse);
    float4 worldPosition = mul(screenPos, InverseViewMatrix);


    return worldPosition.xyz;
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


float4 main(PS_INPUT input) : SV_TARGET
{    
    float4 texturePos = positionTexture.Sample(PointClamp, input.inTexCoord);
    //float3 computedPos = CalculateWorldPositionFromDepthMap(input.inTexCoord);
    float3 textureNormal = normalTexture.Sample(PointClamp, input.inTexCoord);
    float3 textureColor = colorTexture.Sample(PointClamp, input.inTexCoord);
    
    float3 lightVal = AmbientColor * AmbientStrength;
    float3 finalColor = float3(0.0f, 0.0f, 0.0f);
    
    float3 rayDir = normalize(texturePos.xyz - CameraPosition.xyz);
    float cameraToPixelDistance = length(texturePos.xyz - CameraPosition.xyz);
    cameraToPixelDistance = min(cameraToPixelDistance, 1000.0f);

    float scatteredLightIntensity = InScatter(texturePos.xyz, rayDir, spotLight.Position.xyz, cameraToPixelDistance);
    scatteredLightIntensity = pow(scatteredLightIntensity, 0.5f);
    float3 scatterLight = scatteredLightIntensity * spotLight.Color;
    
    if (texturePos.w < 0.0f)
        return float4(textureColor + scatterLight, 1.0f);
    
    float4 lightSpacePos = mul(float4(texturePos.xyz, 1.0f), transpose(spotLight.ViewProjMatrix)); // 도대체 왜 이걸 전치시켜야 하는지 이해를 못하겠다
    lightVal += CalculateShadow(0, lightSpacePos) * CalculateLightColor(texturePos.xyz, textureNormal.xyz);
    
    float3 reflectionVector = reflect(rayDir, textureNormal);
    float3 reflectionColor = skyBoxCube.Sample(PointClamp, reflectionVector) * 0.5;
    
    float ambientOcclusionFactor = ComputeAmbientOcclusion(texturePos.xyz, textureNormal.xyz, input.inTexCoord);
    
    finalColor = scatterLight + saturate(lightVal * textureColor * ambientOcclusionFactor + reflectionColor);
    //finalColor = max(scatterLight, saturate(lightVal * textureColor + reflectionColor));
    return float4(finalColor, 1.0f);
}