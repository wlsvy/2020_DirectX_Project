SamplerState SampleTypePoint : register(s0);
SamplerState SampleTypeWrap : register(s1);

Texture2D positionTexture : register(t0);
Texture2D normalTexture : register(t1);
Texture2D colorTexture : register(t2);
Texture2D lightTexture : register(t3);

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

float4 main(PS_INPUT input) : SV_TARGET
{    
    float4 texturePos = positionTexture.Sample(SampleTypePoint, input.inTexCoord);
    float3 textureNormal = normalTexture.Sample(SampleTypePoint, input.inTexCoord);
    float3 textureColor = colorTexture.Sample(SampleTypePoint, input.inTexCoord);
    float3 textureLight = lightTexture.Sample(SampleTypePoint, input.inTexCoord);
    float3 ambient = textureColor * _ambientColor * _ambientStrength;
    float3 finalColor = float3(0.0f, 0.0f, 0.0f);
    
    if (texturePos.w < 0.0f)
        return float4(textureColor, 1.0f);
    
    finalColor = saturate(textureLight + ambient);
    return float4(finalColor, 1.0f);
}