
SamplerState SampleTypePoint : register(s0);
SamplerState SampleTypeWrap : register(s1);

Texture2D positionTexture : register(t0);
Texture2D normalTexture : register(t1);
Texture2D colorTexture : register(t2);
Texture2D shadowMap : register(t3);

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
cbuffer ShadowMap : register(b2)
{
    float4x4 shadowmapVP;
    float3 lightpos;
    float pad2;
}

struct PS_INPUT
{
    float4 inPosition : SV_POSITION;
    float2 inTexCoord : TEXCOORD0;
    float3 inNormal : NORMAL0;
    float3 inWorldPos : WORLD_POSITION0;
    float4 inLightPosition : TEXCOORD1;
    float4 inToLightDir : TEXCOORD2;
};


float4 main(PS_INPUT input) : SV_TARGET
{
    //float4 lightViewPosition = mul(float4(input.inWorldPos, 1.0f), shadowmapVP);
    //float4 lightViewPosition = input.inLightPosition;
    float4 lightViewPosition = input.inPosition;
    float depth = lightViewPosition.z / lightViewPosition.w;
    if(depth > 1.0f || depth < 0.0f) return float4(1.0f, 0.0f, 1.0f, 1.0f);
    depth *= 0.1f;
    
    float4 output = float4(1.0f, 0.0f, 1.0f, 1.0f);
    //float4 output = float4(1.0f, 0.0f, 1.0f, 1.0f);
    //float4 output = float4(depth, depth, depth, 1.0f);
    //float4 output = float4(shadowMap.Sample(SampleTypePoint, input.inTexCoord).xyz, 1.0f);
    
    
    
    
    float3 vectorToLight = position - input.inWorldPos.xyz;
    float distToLight = length(vectorToLight);
    
    
    vectorToLight /= distToLight;
    //float diffuseLightIntensity = max(dot(vectorToLight, textureNormal), 0.0f);
    //float diffuseLightIntensity = 0.0f;
    
    float2 projectTexCoord;
    
    if (lightViewPosition.w > 0.0f)
    {
        projectTexCoord.x = lightViewPosition.x / lightViewPosition.w * 0.5f + 0.5f;
        projectTexCoord.y = -lightViewPosition.y / lightViewPosition.w * 0.5f + 0.5f;
        
        if ((saturate(projectTexCoord.x) == projectTexCoord.x) && (saturate(projectTexCoord.y) == projectTexCoord.y))
        {
            return float4(depth, depth, depth, 1.0f);
            //return float4(1.0f, 1.0f, 1.0f, 1.0f);
            //float depthValue = shadowMap.Sample(SampleTypeWrap, projectTexCoord).r;
            float depthValue = shadowMap.Sample(SampleTypeWrap, input.inTexCoord).r;

            float lightDepthValue = lightViewPosition.z / lightViewPosition.w;

        //lightDepthValue = lightDepthValue - 0.001;

            if (lightDepthValue < depthValue)
            {
                return float4(0.0f, 1.0f, 1.0f, 1.0f);
                //return shadowMap.Sample(SampleTypeWrap, projectTexCoord);
            //diffuseLightIntensity = saturate(dot(textureNormal, texturePos.xyz));
                float diffuseLightIntensity = max(dot(vectorToLight, input.inNormal), 0.0f);
                if (diffuseLightIntensity > 0.0f)
                {
                    float attFactor = 1 / (attentuation.x + (attentuation.y * distToLight) + (attentuation.z * pow(distToLight, 2)));
                    float spotFactor = pow(max(dot(-vectorToLight, forwardVector), 0.0f), 1.0f);
                    //finalColor = textureColor * color * strength * spotFactor * attFactor;
                    //return float4(color, 1.0f) * strength * spotFactor * attFactor;

                }
            }
        }
    }
    
    return output;
}