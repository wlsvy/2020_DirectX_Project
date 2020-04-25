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
    float3 ambient = textureColor * ambientColor * ambientStrength;
    float3 finalColor = float3(0.0f, 0.0f, 0.0f);
    float3 vectorToLight = position - texturePos.xyz;
    float distToLight = length(vectorToLight);
    if (texturePos.w < 0.0f)
        return float4(textureColor, 1.0f);
    if (distToLight > range)
        return float4(ambient, 1.0f);
    
    
    vectorToLight /= distToLight;
    //float diffuseLightIntensity = max(dot(vectorToLight, textureNormal), 0.0f);
    //float diffuseLightIntensity = 0.0f;
    float4 lightViewPosition = mul(texturePos, lightVPmatrix);
    float2 projectTexCoord;
    
    if (lightViewPosition.w > 0.0f)
    {
        projectTexCoord.x = lightViewPosition.x / lightViewPosition.w * 0.5f + 0.5f;
        projectTexCoord.y = lightViewPosition.y / lightViewPosition.w * 0.5f + 0.5f;
        
        if ((saturate(projectTexCoord.x) == projectTexCoord.x) && (saturate(projectTexCoord.y) == projectTexCoord.y))
        {
            return float4(1.0f, 1.0f, 1.0f, 1.0f);
            float depthValue = shadowMap.Sample(SampleTypeWrap, projectTexCoord).r;

            float lightDepthValue = lightViewPosition.z / lightViewPosition.w;

        //lightDepthValue = lightDepthValue - 0.001;

            if (lightDepthValue < depthValue)
            {
                return shadowMap.Sample(SampleTypeWrap, projectTexCoord);
            //diffuseLightIntensity = saturate(dot(textureNormal, texturePos.xyz));
                float diffuseLightIntensity = max(dot(vectorToLight, textureNormal), 0.0f);
                if (diffuseLightIntensity > 0.0f)
                {
                    float attFactor = 1 / (attentuation.x + (attentuation.y * distToLight) + (attentuation.z * pow(distToLight, 2)));
                    float spotFactor = pow(max(dot(-vectorToLight, forwardVector), 0.0f), 1.0f);
                    finalColor = textureColor * color * strength * spotFactor * attFactor;
                }
            }
        }
    }
    
    
    
    //if (diffuseLightIntensity > 0.0f)
    //{
    //    float attFactor = 1 / (attentuation.x + (attentuation.y * distToLight) + (attentuation.z * pow(distToLight, 2)));
    //    float spotFactor = pow(max(dot(-vectorToLight, forwardVector), 0.0f), spotAngle);
    //    finalColor = textureColor * color * strength * spotFactor * attFactor;
    //}
    
    finalColor = saturate(finalColor + ambient);
    return float4(finalColor, 1.0f);
    //return shadowMap.Sample(SampleTypePoint, projectTexCoord);

}