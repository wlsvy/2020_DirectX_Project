Texture2D positionTexture : register(t0);
Texture2D normalTexture : register(t1);
Texture2D colorTexture : register(t2);
SamplerState SampleTypePoint : register(s0);

cbuffer lightBuffer : register(b0)
{
    float3 position;
    float range;
    
    float3 forwardVector;
    float spotAngle;
    
    float3 color;
    float strength;
    
    float3 ambientColor;
    float ambientStrength;
    
    float3 attentuation;
    float pad;
}

struct PS_INPUT
{
    float4 inPosition : SV_POSITION;
    float2 inTexCoord : TEXCOORD;
    float3 inNormal : NORMAL;
    float3 inWorldPos : WORLD_POSITION;
};

float4 main(PS_INPUT input) : SV_TARGET
{    
    float4 texturePos = positionTexture.Sample(SampleTypePoint, input.inTexCoord);
    float3 textureNormal = normalTexture.Sample(SampleTypePoint, input.inTexCoord);
    float3 textureColor = colorTexture.Sample(SampleTypePoint, input.inTexCoord);
    float3 ambient = textureColor * ambientColor * ambientStrength;
    float3 finalColor = float3(0.0f, 0.0f, 0.0f);
    float3 vectorToLight = position - texturePos.xyz;
    float distToLight = length(vectorToLight);
    if (texturePos.w < 0.0f)
        return float4(textureColor, 1.0f);
    if (distToLight > range)
        return float4(ambient, 1.0f);
    
    vectorToLight /= distToLight;
    float diffuseLightIntensity = max(dot(vectorToLight, textureNormal), 0.0f);
    
    if (diffuseLightIntensity > 0.0f)
    {
        float attFactor = 1 / (attentuation.x + (attentuation.y * distToLight) + (attentuation.z * pow(distToLight, 2)));
        float spotFactor = pow(max(dot(-vectorToLight, forwardVector), 0.0f), spotAngle);
        finalColor = textureColor * color * strength * spotFactor * attFactor;
    }
    
    finalColor = saturate(finalColor + ambient);
    return float4(finalColor, 1.0f);
}