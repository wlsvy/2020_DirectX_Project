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

float3 CalculateWorldPositionFromDepthMap(float4 pos)
{
    float depth = pos.z / pos.w;
    
    float2 screenCoord = 0.5f * pos.xy / pos.w + 0.5f;
    screenCoord.y = 1.0f - screenCoord.y;

    // Translate from homogeneous coords to texture coords.
    //float2 depthTexCoord = 0.5f * screenCoord + 0.5f;
    //depthTexCoord.y = 1.0f - depthTexCoord.y;
    
    
    //float depth = depthTexture.Sample(PointClamp, depthTexCoord).r;

    float4 screenPos = float4(screenCoord.x, screenCoord.y, depth, 1.0);
    float4 viewPosition = mul(transpose(InverseProjMatrix), screenPos);
    //viewPosition /= viewPosition.w; // Perspective division
    //float4 worldPosition = mul(viewPosition, viewInverse);
    float4 worldPosition = mul(transpose(InverseViewMatrix), screenPos);


    return worldPosition.xyz;
}


PS_OUTPUT main(PS_INPUT input) : SV_TARGET
{
    PS_OUTPUT output;
    //float4x4 inverseVP = mul(InverseProjMatrix, InverseViewMatrix);
    output.pos = float4(input.inWorldPos, 1.0f);    
    //output.pos = mul(input.inPosition, transpose(inverseVP));
    //output.pos = float4(mul(mul(input.inPosition, transpose(InverseProjMatrix)), transpose(InverseProjMatrix)).xyz, 1.0f);
    //output.pos = float4(float(input.inPosition.z / input.inPosition.w).xxx, 1.0f);
    //output.pos = float4(CalculateWorldPositionFromDepthMap(input.inPosition), 1.0f);
    output.color = objTexture.Sample(PointClamp, input.inTexCoord) * materialColor;
    
    float3 calNormal = CalcPerPixelNormal(input.inTexCoord, input.inNormal, input.inTangent.xyz);
    //output.normal = float4(calNormal, 1.0f);
    output.normal = float4(input.inNormal, 1.0f);
    
    float4 lightSpacePos = mul(float4(input.inWorldPos, 1.0f), transpose(spotLight.ViewProjMatrix)); // 도대체 왜 이걸 전치시켜야 하는지 이해를 못하겠다
    output.light = CalculateShadow(0, lightSpacePos) * CalculateLightColor(input.inWorldPos.xyz, input.inNormal);
    
    float depth = input.inPosition.z / input.inPosition.w;
    output.depth = float4(input.inDepth.xxx, 1.0f);
    return output;
}