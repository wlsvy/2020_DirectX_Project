#include "Include/Common.hlsli"
#include "Include/Shadow.hlsli"

void ProcessVl_ShadowTexure(float2 uv, float2 size, float depth, out float3 volumetricLight, out float shadow)
{
    float4 result = float4(0.0f, 0.0f, 0.0f, 0.0f);
    float linearLightDepth = lightShadowTexture.Sample(PointClamp, uv).z;
    
    [unroll(50)]
    for (float x = -2; x <= 2; x += 1.0f)
    {
        for (float y = -2; y <= 2; y += 1.0f)
        {
            //float2 offset = float2(x, y) / size * (1 - linearLightDepth) * softShadowPCFBias;
            float2 offset = float2(x, y) / size * softShadowPCFBias;
            //result += lightShadowTexture.Sample(LinearWrap, uv + offset);
            
            float2 texelSize = float2(1.0f, 1.0f) / size * softShadowInterpoloateBias;
    
            half4 lb = lightShadowTexture.Sample(PointClamp, uv + offset + texelSize * float2(0.0f, 0.0f));
            half4 lt = lightShadowTexture.Sample(PointClamp, uv + offset + texelSize * float2(0.0f, 1.0f));
            half4 rb = lightShadowTexture.Sample(PointClamp, uv + offset + texelSize * float2(1.0f, 0.0f));
            half4 rt = lightShadowTexture.Sample(PointClamp, uv + offset + texelSize * float2(1.0f, 1.0f));
    
            result += (lb + lt + rb + rt) * 0.25 * half4(1.0f, 1 - linearLightDepth, 1.0f, 1.0f);
        }
    }
    
    result *= 0.04;
    volumetricLight = result.x * spotLight.Color;
    shadow = result.y;
}

float4 main(Vertex_Quad input) : SV_TARGET
{
    float4 position = positionTexture.Sample(PointClamp, input.inTexCoord);
    float3 normal = normalTexture.Sample(PointClamp, input.inTexCoord).xyz;
    float3 albedo = colorTexture.Sample(PointClamp, input.inTexCoord).xyz;
    float3 matProperty = matPropertyTexture.Sample(PointClamp, input.inTexCoord).xyz;
    float depth = depthTexture.Sample(PointClamp, input.inTexCoord).x;
    float metal = matProperty.x;
    float roughness = matProperty.y;
    float specular = matProperty.z;
    float ambientOcclusion = ssaoTexture.Sample(PointClamp, input.inTexCoord).x;
    
    float3 volumetricLight;
    float shadow;
    ProcessVl_ShadowTexure(input.inTexCoord, float2(1024, 768), depth, volumetricLight, shadow);
    //return float4(shadow.xxx, 1.0f);
    float3 lightVal = AmbientColor * AmbientStrength * ambientOcclusion;
    lightVal += CalculateLightColor(position.xyz, normal.xyz).xyz * shadow;
    float3 finalColor = float3(0.0f, 0.0f, 0.0f);
    
    float3 rayDir = normalize(position.xyz - CameraPosition.xyz);
    float3 reflectionVector = reflect(rayDir, normal);
    float3 reflectionColor = irradianceTexture.Sample(PointClamp, reflectionVector).xyz * 0.5;
    
    if (position.w < 0.0f)
    {
        return float4(albedo + volumetricLight, 1.0f);
    }
       
    finalColor = volumetricLight + saturate(lightVal * albedo + reflectionColor);
    return float4(finalColor, 1.0f);
}