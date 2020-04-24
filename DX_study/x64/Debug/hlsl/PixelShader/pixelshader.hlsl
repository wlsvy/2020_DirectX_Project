cbuffer lightBuffer : register(b0)
{
    float3 ambientLightColor;
    float ambientLightStrength;

    float3 dynamicLightColor;
    float dynamicLightStrength;
    float3 dynamicLightPosition;

    float dynamiLightAttenuation_a;
    float dynamiLightAttenuation_b;
    float dynamiLightAttenuation_c;
    float2 b0pad;
}

struct PS_INPUT
{
    float4 inPosition : SV_POSITION;
    //float3 inColor : COLOR;
    float2 inTexCoord : TEXCOORD;
    float3 inNormal : NORMAL;       //정점 상의 노말 벡터를 보간하면, 다시 정규화를 해줘야 할 필요가 있음. 
    float3 inWorldPos : WORLD_POSITION;
};

Texture2D objTexture : TEXTURE : register(t0);
SamplerState objSamplerState : SAMPLER : register(s0);

float4 main(PS_INPUT input) : SV_TARGET
{
    float3 sampleColor = objTexture.Sample(objSamplerState, input.inTexCoord);
    float3 ambientLight = ambientLightColor * ambientLightStrength;
    float3 vectorToLight = normalize(dynamicLightPosition - input.inWorldPos);
    float3 diffuseLightIntensity = max(dot(vectorToLight, input.inNormal), 0);
    float distanceToLight = distance(dynamicLightPosition, input.inWorldPos);
    float attenuationFactor = 1 / (dynamiLightAttenuation_a + dynamiLightAttenuation_b * distanceToLight + dynamiLightAttenuation_c * pow(distanceToLight, 2));
    diffuseLightIntensity *= attenuationFactor;
    float3 diffuseLight = diffuseLightIntensity * dynamicLightStrength * dynamicLightColor;

    return float4(sampleColor * (ambientLight + diffuseLight), 1.0f);
}

