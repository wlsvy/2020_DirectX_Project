SamplerState SampleTypePoint : register(s0);
SamplerState SampleTypeWrap : register(s1);

Texture2D positionTexture : register(t0);
Texture2D normalTexture : register(t1);
Texture2D colorTexture : register(t2);
//Texture2D lightTexture : register(t3);
Texture2D shadowMap :  register(t3);


cbuffer lightBuffer : register(b0)
{
    float3 position;
    float range;
    
    float3 forwardVector;
    float spotAngle;
    
    float3 lightColor;
    float strength;
    
    float3 ambientColor;
    float ambientStrength;
    
    float3 attentuation;
    float pad;
    
    float4x4 lightVPmatrix;
}

cbuffer AmbientLight : register(b2)
{
    float3 _ambientColor;
    float _ambientStrength;
    
}

struct PS_INPUT
{
    float4 inPosition : SV_POSITION;
    float2 inTexCoord : TEXCOORD;
};

float4 CalculateShadow(int lightIndex, float4 lightSpacePos)
{
    float lightDepth = 1 - lightSpacePos.z / range + 0.01;
    
    float2 projectTexCoord = 0.5f * lightSpacePos.xy / lightSpacePos.w + 0.5f;
    projectTexCoord.y = 1.0f - projectTexCoord.y;
        
    if (projectTexCoord.x <= 0.0f || projectTexCoord.x >= 1.0f || projectTexCoord.y <= 0.0f || projectTexCoord.y >= 1.0f || lightDepth > 1.0f)
    {
        return float4(1.0f, 1.0f, 1.0f, 1.0f);
    }
    
    float shadowMapDepth = shadowMap.Sample(SampleTypeWrap, projectTexCoord).r;
    
    if (lightDepth >= shadowMapDepth)
    {
        return float4(1.0f, 1.0f, 1.0f, 1.0f);
    }
    return float4(0.0f, 0.0f, 0.0f, 0.0f);
}

float4 CalculateLightColor(float3 worldPos, float3 normal)
{
    float3 vectorToLight = position - worldPos;
    float distToLight = length(vectorToLight);
    vectorToLight /= distToLight;
    float diffuseLightIntensity = max(dot(vectorToLight, normal), 0.0f);
    if (diffuseLightIntensity > 0.0f)
    {
        float attFactor = 1 / (attentuation.x + (attentuation.y * distToLight) + (attentuation.z * pow(distToLight, 2)));
        float spotFactor = pow(max(dot(-vectorToLight, forwardVector), 0.0f), 1.0f);
        return float4(lightColor, 1.0f) * strength * spotFactor * attFactor;

    }
    
    return float4(0.0f, 0.0f, 0.0f, 1.0f);
}

float4 main(PS_INPUT input) : SV_TARGET
{    
    float4 texturePos = positionTexture.Sample(SampleTypePoint, input.inTexCoord);
    float3 textureNormal = normalTexture.Sample(SampleTypePoint, input.inTexCoord);
    float3 textureColor = colorTexture.Sample(SampleTypePoint, input.inTexCoord);
    //float3 textureLight = lightTexture.Sample(SampleTypePoint, input.inTexCoord);
    float3 ambient = textureColor * _ambientColor * _ambientStrength;
    float3 finalColor = float3(0.0f, 0.0f, 0.0f);
    
    if (texturePos.w < 0.0f)
        return float4(textureColor, 1.0f);
    
    float4 lightSpacePos = mul(float4(texturePos.xyz, 1.0f), transpose(lightVPmatrix)); // 도대체 왜 이걸 전치시켜야 하는지 이해를 못하겠다
    float3 textureLight = CalculateShadow(0, lightSpacePos) * CalculateLightColor(texturePos.xyz, textureNormal.xyz);
    
    finalColor = saturate(textureLight * textureColor + ambient);
    return float4(finalColor, 1.0f);
}