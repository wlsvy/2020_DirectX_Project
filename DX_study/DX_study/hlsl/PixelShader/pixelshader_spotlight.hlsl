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
    //float3 inColor : COLOR;
    float2 inTexCoord : TEXCOORD;
    float3 inNormal : NORMAL; //정점 상의 노말 벡터를 보간하면, 다시 정규화를 해줘야 할 필요가 있음. 
    float3 inWorldPos : WORLD_POSITION;
};

Texture2D objTexture : TEXTURE : register(t0);
SamplerState objSamplerState : SAMPLER : register(s0);

float4 main(PS_INPUT input) : SV_TARGET
{
    float3 sampleColor = objTexture.Sample(objSamplerState, input.inTexCoord);
    float3 ambient = sampleColor * ambientColor * ambientStrength;
    float3 finalColor = float3(0.0f, 0.0f, 0.0f);
    float3 vectorToLight = position - input.inWorldPos;
    float distToLight = distance(position, input.inWorldPos);
    if (distToLight > range)
        return float4(ambient, 1.0f);
    
    vectorToLight /= distToLight;
    float diffuseLightIntensity = max(dot(vectorToLight, input.inNormal), 0.0f);
    
    if (diffuseLightIntensity > 0.0f)
    {
        float attFactor = 1 / (attentuation.x + (attentuation.y * distToLight) + (attentuation.z * pow(distToLight, 2)));
        float spotFactor = pow(max(dot(-vectorToLight, forwardVector), 0.0f), spotAngle);
        finalColor = sampleColor * color * strength * spotFactor * attFactor;
    }
    
    finalColor = saturate(finalColor + ambient);
    return float4(finalColor, 1.0f);
}

