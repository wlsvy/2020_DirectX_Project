cbuffer cameraBuffer : register(b1)
{
    float3 dynamicCameraPosition;
    float b1pad;
}

cbuffer SimpleLightBuffer : register(b2)
{
    float3 ambientLightColor;
    float ambientLightStrength;
}

cbuffer fogBuffer : register(b3)
{
    float3 fogColor;
    float fogStart;
    float fogRange;
    int fogEnabled;
    float b2pad;
}

struct PS_INPUT
{
    float4 inPosition : SV_POSITION;
    float2 inTexCoord : TEXCOORD;
    float3 inNormal : NORMAL;
    float3 inWorldPos : WORLD_POSITION;
};

Texture2D objTexture : TEXTURE : register(t0);
SamplerState objSamplerState : SAMPLER : register(s0);

float4 main(PS_INPUT input) : SV_TARGET
{
    float distToEye = distance(dynamicCameraPosition, input.inWorldPos);

    float3 sampleColor = objTexture.Sample(objSamplerState, input.inTexCoord);
    float3 ambientLight = ambientLightColor * ambientLightStrength;
    float3 finalColor = sampleColor * ambientLight;

    if (fogEnabled == 1)
    {
        float fogLerp = saturate((distToEye - fogStart) / fogRange);
        finalColor = lerp(finalColor, fogColor, fogLerp);
    }

    finalColor = input.inNormal;
    return float4(finalColor, 1.0f);
}

