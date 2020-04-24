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

struct PS_OUTPUT
{
    float4 pos : SV_Target0;
    float4 normal : SV_Target1;
    float4 color : SV_Target2;
};

Texture2D objTexture : TEXTURE : register(t0);
SamplerState objSamplerState : SAMPLER : register(s0);

float4 main(PS_INPUT input) : SV_TARGET
{
    float3 textureColor = objTexture.Sample(objSamplerState, input.inTexCoord);
    float3 ambient = textureColor * ambientColor * ambientStrength;
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
        finalColor = textureColor * color * strength * spotFactor * attFactor;
    }
    
    finalColor = saturate(finalColor + ambient);
    return float4(finalColor, 1.0f);
}

