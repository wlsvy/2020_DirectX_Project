struct PS_INPUT
{
    float4 inPosition : SV_POSITION0;
    float2 inTexCoord : TEXCOORD0;
    float3 inNormal : NORMAL0;
    float3 inWorldPos : WORLD_POSITION0;
    float4 inLightPosition : TEXCOORD1;
    float4 inToLightDir : TEXCOORD2;
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
Texture2D shadowMap : TEXTURE : register(t1);

SamplerState objSamplerState : SAMPLER : register(s0);
SamplerState objSamplerState1 : SAMPLER : register(s1);

PS_OUTPUT main(PS_INPUT input) : SV_TARGET
{
    PS_OUTPUT output;
    output.pos = float4(input.inWorldPos, 1.0f);
    output.normal = float4(input.inNormal, 1.0f);
    output.color = objTexture.Sample(objSamplerState, input.inTexCoord) * materialColor;
    output.light = float4(0.0f, 0.0f, 0.0f, 1.0f);
    //float lightDepthValue = input.inLightPosition.z / input.inLightPosition.w;
    float4 lightviewpos = mul(output.pos, lightVPmatrix);
    
    //if (length(lightviewpos - input.inLightPosition) < 0.1)
    //{
    //    output.light = float4(1.0f, 1.0f, 1.0f, 1.0f);
    //}
    
    //lightDepthValue = lightviewpos.z / lightviewpos.w;
    //output.light = float4(input.inToLightDir.xyz, 1.0f);
    //output.light = float4(normalize(position - output.pos.xyz), 1.0f);
    //output.light = lightviewpos;
    //output.light = shadowMap.Sample(objSamplerState, input.inTexCoord);
    //output.light = float4(input.inWorldPos, 1.0f);
    //output.light = float4(input.inLightPosition.w, 0.0f, 0.0f, 1.0f);
    //output.light = float4(normalize(position - input.inWorldPos.xyz), 1.0f);
    //output.light = float4(input.inToLightDir.xyz, 1.0f);
    
   
    
    float2 projectTexCoord;
    projectTexCoord.x = input.inLightPosition.x / input.inLightPosition.w / 2.0f + 0.5f;
    projectTexCoord.y = -input.inLightPosition.y / input.inLightPosition.w / 2.0f + 0.5f;
    //projectTexCoord.x = lightviewpos.x / lightviewpos.w / 2.0f + 0.5f;
    //projectTexCoord.y = -lightviewpos.y / lightviewpos.w / 2.0f + 0.5f;
    if ((saturate(projectTexCoord.x) == projectTexCoord.x) && (saturate(projectTexCoord.y) == projectTexCoord.y))
    {
        //output.light = float4(1.0f, 0.0f, 1.0f, 1.0f);
        float depthValue = shadowMap.Sample(objSamplerState1, projectTexCoord).r;
        //float lightDepthValue = lightviewpos.z / lightviewpos.w;
        float lightDepthValue = input.inLightPosition.z / input.inLightPosition.w;

        //lightDepthValue = lightDepthValue - 0.001;

        if (lightDepthValue > depthValue)
        {
            //output.light = float4(1.0f, 0.0f, 0.0f, 1.0f);
            output.light = float4(lightDepthValue, lightDepthValue, lightDepthValue, 1.0f);
            //return shadowMap.Sample(objSamplerState, projectTexCoord);
            //diffuseLightIntensity = saturate(dot(textureNormal, texturePos.xyz));
            //float diffuseLightIntensity = max(dot(vectorToLight, textureNormal), 0.0f);
            //if (diffuseLightIntensity > 0.0f)
            //{
            //    float attFactor = 1 / (attentuation.x + (attentuation.y * distToLight) + (attentuation.z * pow(distToLight, 2)));
            //    float spotFactor = pow(max(dot(-vectorToLight, forwardVector), 0.0f), 1.0f);
            //    finalColor = textureColor * color * strength * spotFactor * attFactor;
            //}
        }
    }
    
    return output;
}