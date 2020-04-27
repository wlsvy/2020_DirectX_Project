#include "../pixelshader_Common.hlsli"

Texture2D positionTexture : register(t0);
Texture2D normalTexture : register(t1);
Texture2D colorTexture : register(t2);
//Texture2D lightTexture : register(t3);
Texture2D shadowMap :  register(t3);



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


float4 main(PS_INPUT input) : SV_TARGET
{    
    float4 texturePos = positionTexture.Sample(PointClamp, input.inTexCoord);
    float3 textureNormal = normalTexture.Sample(PointClamp, input.inTexCoord);
    float3 textureColor = colorTexture.Sample(PointClamp, input.inTexCoord);
    //float3 textureLight = lightTexture.Sample(SampleTypePoint, input.inTexCoord);
    float3 ambient = textureColor * AmbientColor * AmbientStrength;
    float3 finalColor = float3(0.0f, 0.0f, 0.0f);
    
    float3 rayDir = texturePos.xyz - CameraPosition.xyz;
    float cameraToPixelDistance = length(rayDir);
    cameraToPixelDistance = min(cameraToPixelDistance, 1000.0f);
    rayDir /= cameraToPixelDistance;
    float scatteredLightIntensity = InScatter(texturePos.xyz, rayDir, spotLight.Position.xyz, cameraToPixelDistance);
    scatteredLightIntensity = pow(scatteredLightIntensity, 0.25f);
    float3 scatterLight = scatteredLightIntensity * spotLight.Color;
    
    if (texturePos.w < 0.0f)
        return float4(textureColor + scatterLight, 1.0f);
    
    float4 lightSpacePos = mul(float4(texturePos.xyz, 1.0f), transpose(spotLight.ViewProjMatrix)); // 도대체 왜 이걸 전치시켜야 하는지 이해를 못하겠다
    float3 textureLight = CalculateShadow(0, lightSpacePos) * CalculateLightColor(texturePos.xyz, textureNormal.xyz);
    
    finalColor = saturate(scatterLight + textureLight * textureColor + ambient);
    return float4(finalColor, 1.0f);
}