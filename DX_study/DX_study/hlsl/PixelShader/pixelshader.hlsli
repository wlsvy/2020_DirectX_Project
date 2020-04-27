
struct PS_INPUT
{
    float4 inPosition : SV_POSITION0;
    float2 inTexCoord : TEXCOORD0;
    float3 inNormal : NORMAL0;
    float3 inWorldPos : WORLD_POSITION0;
    float4 inTangent : TANGENT;
};

struct PS_OUTPUT
{
    float4 pos : WORLD_POSITION;
    float4 normal : NORMAL;
    float4 color : COLOR0;
    float4 light : COLOR1;
};

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

cbuffer Material : register(b1)
{
    float4 materialColor;
};

Texture2D objTexture : TEXTURE : register(t0);
Texture2D normalMap : TEXTURE : register(t1);
Texture2D shadowMap : TEXTURE : register(t3);

SamplerState objSamplerState : SAMPLER : register(s0);
SamplerState objSamplerState1 : SAMPLER : register(s1);

float3 CalcPerPixelNormal(float2 vTexcoord, float3 vVertNormal, float3 vVertTangent)
{
    // Compute tangent frame.
    vVertNormal = normalize(vVertNormal);
    vVertTangent = normalize(vVertTangent);

    float3 vVertBinormal = normalize(cross(vVertTangent, vVertNormal));
    float3x3 mTangentSpaceToWorldSpace = float3x3(vVertTangent, vVertBinormal, vVertNormal);

    // Compute per-pixel normal.
    float3 vBumpNormal = (float3) normalMap.Sample(objSamplerState1, vTexcoord);
    vBumpNormal = 2.0f * vBumpNormal - 1.0f;

    return mul(vBumpNormal, mTangentSpaceToWorldSpace);
}


//float4 CalculateShadow(int lightIndex, float4 lightSpacePos)
//{
//    float lightDepth = 1 - lightSpacePos.z / range + 0.01;
    
//    float2 projectTexCoord = 0.5f * lightSpacePos.xy / lightSpacePos.w + 0.5f;
//    projectTexCoord.y = 1.0f - projectTexCoord.y;
        
//    if (projectTexCoord.x <= 0.0f || projectTexCoord.x >= 1.0f || projectTexCoord.y <= 0.0f || projectTexCoord.y >= 1.0f || lightDepth > 1.0f)
//    {
//        return float4(1.0f, 1.0f, 1.0f, 1.0f);
//    }
    
//    float shadowMapDepth = shadowMap.Sample(objSamplerState1, projectTexCoord).r;
    
//    if (lightDepth >= shadowMapDepth)
//    {
//        return float4(1.0f, 1.0f, 1.0f, 1.0f);
//    }
//    return float4(0.0f, 0.0f, 0.0f, 0.0f);
//}


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