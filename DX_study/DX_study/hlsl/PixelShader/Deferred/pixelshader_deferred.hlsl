#include "../pixelshader_ObjectCommon.hlsli"


float4 CalculateShadow(int lightIndex, float4 lightSpacePos)
{
    float lightDepth = 1 - lightSpacePos.z / spotLight.Range + 0.01;
    
    float2 projectTexCoord = 0.5f * lightSpacePos.xy / lightSpacePos.w + 0.5f;
    projectTexCoord.y = 1.0f - projectTexCoord.y;
        
    if (projectTexCoord.x <= 0.0f || projectTexCoord.x >= 1.0f || projectTexCoord.y <= 0.0f || projectTexCoord.y >= 1.0f || lightDepth > 1.0f)
    {
        return float4(1.0f, 1.0f, 1.0f, 1.0f);
    }
    
    float shadowMapDepth = shadowMap.Sample(LinearWrap, projectTexCoord).r;
    
    if (lightDepth >= shadowMapDepth)
    {
        return float4(1.0f, 1.0f, 1.0f, 1.0f);
    }
    return float4(0.0f, 0.0f, 0.0f, 0.0f);
}


PS_OUTPUT main(PS_INPUT input) : SV_TARGET
{
    PS_OUTPUT output;
    output.pos = float4(input.inWorldPos, 1.0f);
    output.color = objTexture.Sample(PointClamp, input.inTexCoord) * materialColor;
    
    float3 calNormal = CalcPerPixelNormal(input.inTexCoord, input.inNormal, input.inTangent.xyz);
    //output.normal = float4(calNormal, 1.0f);
    output.normal = float4(input.inNormal, 1.0f);
    
    output.materialProperty = float4(
    metalMap.Sample(PointClamp, input.inTexCoord).r * MetalIntensity,
    roughnessMap.Sample(PointClamp, input.inTexCoord).r * RoughnessIntensity,
    specularMap.Sample(PointClamp, input.inTexCoord).r * SpecularIntensity,
    1.0f);
    
    float depth = input.inPosition.z / input.inPosition.w;
    output.depth = float4(depth, depth, depth, 1.0f);
    return output;
}