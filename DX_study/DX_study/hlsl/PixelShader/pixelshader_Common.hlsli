static const float PI = 3.14159265358979323846;

SamplerState PointClamp : SAMPLER : register(s0);
SamplerState LinearWrap : SAMPLER : register(s1);

cbuffer CB_Scene : register(b0)
{
    float3 CameraPosition;
    float pad_Scene;
    
    float3 AmbientColor;
    float AmbientStrength;
    
    float4x4 InverseViewMatrix;
    float4x4 InverseProjMatrix;
}

struct SpotLight
{
    float3 Position;
    float Range;

    float3 Forward;
    float SpotAngle;

    float3 Color;
    float Strength;
    
    float3 Attenuation;
    float pad;
    
    float4x4 ViewProjMatrix;
};

cbuffer CB_SpotLight : register(b1)
{
    SpotLight spotLight;
}


float4 CalculateLightColor(float3 worldPos, float3 normal)
{
    float3 vectorToLight = spotLight.Position - worldPos;
    float distToLight = length(vectorToLight);
    vectorToLight /= distToLight;
    float diffuseLightIntensity = max(dot(vectorToLight, normal), 0.0f);
    if (diffuseLightIntensity > 0.0f)
    {
        float attFactor = 1 / (spotLight.Attenuation.x + (spotLight.Attenuation.y * distToLight) + (spotLight.Attenuation.z * pow(distToLight, 2)));
        float spotFactor = pow(max(dot(-vectorToLight, spotLight.Forward), 0.0f), 1.0f);
        return float4(spotLight.Color, 1.0f) * spotLight.Strength * spotFactor * attFactor;

    }
    
    return float4(0.0f, 0.0f, 0.0f, 1.0f);
}
