#define MAX_DIRECTIONAL_LIGHT 2
#define MAX_POINT_LIGHT 4
#define MAX_SPOT_LIGHT 4

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
    float specularPow;
    float b0pad;
}

cbuffer cameraBuffer : register(b1)
{
    float3 dynamicCameraPosition;
}

struct CB_LIGHT_Directional
{
    float3 lightVec;
    float lightStrength; //16
    float3 lightColor;
    float pad; //16
};

struct CB_LIGHT_Point
{
    float3 lightColor;
    float lightStrength; //16
    float3 lightPos;
    float lightRange; //16
    float3 Attentuation;
    float pad; //16
};

struct CB_LIGHT_Spot
{
    float3 lightColor;
    float lightStrength; //16
    float3 lightPos;
    float lightRange; //16
    float3 Attentuation;
    float spotAngle; //16
};

cbuffer lightBuffer : register(b5)
{
    CB_LIGHT_Directional directionalLight[MAX_DIRECTIONAL_LIGHT];
    CB_LIGHT_Point pointLight[MAX_POINT_LIGHT];
    CB_LIGHT_Spot spotLight[MAX_SPOT_LIGHT];
}

cbuffer materialBuffer : register(b6)
{

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
    float3 sampleColor = objTexture.Sample(objSamplerState, input.inTexCoord);
    float3 ambientLight = ambientLightColor * ambientLightStrength;

    float3 finalLight = ambientLight;
    
    
    float3 lightVec = normalize(directionalLight[0].lightVec) * -1;
    float3 toEye = normalize(dynamicCameraPosition - input.inWorldPos);
    float diffuseFactor = dot(lightVec, input.inNormal);

    [flatten]
    if (diffuseFactor > 0.0f)
    {
        float3 reflectVec = reflect(-lightVec, input.inNormal);
        float specFactor = pow(max(dot(reflectVec, toEye), 0.0f), specularPow);
        float3 diffuseLight = diffuseFactor * directionalLight[0].lightColor * directionalLight[0].lightStrength;
        float3 specLight = specFactor * float4(1.0f, 1.0f, 1.0f, 1.0f);
        finalLight += diffuseLight + specLight;
    }
    
    float3 finalColor = sampleColor * finalLight;

    return float4(finalColor, 1.0f);
}