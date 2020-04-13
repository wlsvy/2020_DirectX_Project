//cbuffer lightBuffer : register(b1)
//{
//    float3 dynamicLightPosition;
//    float range;
    
//    float3 dynamic_Dir;
//    float cone;
    
//    float3 dynamicLightColor;
//    float dynamicLightStrength;
    
//    float3 attentuation;
//    float b0pad;
    
//    float3 ambientLightColor;
//    float ambientLightStrength;
    
//}

cbuffer ambientLight : register(b0)
{
    float3 ambientLightColor;
    float ambientLightStrength;
}

cbuffer dynamicLight : register(b1)
{
    float3 dynamicLightColor;
    float dynamicLightStrength;
    
    float3 attentuation;
    float b0pad;
}

cbuffer spotLight : register(b2)
{
    float3 dynamicLightPosition;
    float range;
    
    float3 dynamic_Dir;
    float cone;
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
    float3 ambientLight = ambientLightColor * ambientLightStrength;
    float3 appliedLight = ambientLight;
    float3 finalColor = float3(0.0f, 0.0f, 0.0f);
    float3 finalAmbient = sampleColor * ambientLight;
    float3 vectorToLight = dynamicLightPosition - input.inWorldPos;
    float distToLight = distance(dynamicLightPosition, input.inWorldPos);
    if (distToLight > range)
        return float4(finalAmbient, 1.0f);
    //[flatten]
    //if (range > 5.0f)
    //    return float4(sampleColor, 1.0f);
    //return float4(sampleColor * ambientLight, 1.0f);
    
    vectorToLight /= distToLight;
    //vectorToLight = normalize(dynamicLightPosition - input.inWorldPos);
    
    float diffuseLightIntensity = dot(vectorToLight, input.inNormal);
    
    if (diffuseLightIntensity > cone)
    {
        finalColor += sampleColor * dynamicLightColor;
                    
        //Calculate Light's Distance Falloff factor
        finalColor /= (attentuation.x + (attentuation.y * distToLight)) + (attentuation.z * pow(distToLight, 2));

        //Calculate falloff from center to edge of pointlight cone
        finalColor *= pow(max(dot(-vectorToLight, dynamic_Dir), 0.0f), dynamicLightStrength);
    
    }
    
     //make sure the values are between 1 and 0, and add the ambient
    finalColor = saturate(finalColor + finalAmbient);
    //return float4(sampleColor * ambientLight, 1.0f);
    //Return Final Color
    return float4(finalColor, 1.0f);
    
    //float distanceToLight = distance(dynamicLightPosition, input.inWorldPos);
    //float attenuationFactor = 1 / (dynamiLightAttenuation_a + dynamiLightAttenuation_b * distanceToLight + dynamiLightAttenuation_c * pow(distanceToLight, 2));
    //diffuseLightIntensity *= attenuationFactor;
    //float3 diffuseLight = diffuseLightIntensity * dynamicLightStrength * dynamicLightColor;

    //appliedLight += diffuseLight;

    //float3 finalColor = sampleColor * appliedLight;
    //return float4(finalColor, 1.0f);
}

