#include "Include/Common.hlsli"

half3 Prefilter(half3 c)
{
    half brightness = max(c.r, max(c.g, c.b));
    half contribution = max(0, brightness - VolumetricLightVar.z);
    contribution /= max(brightness, 0.00001);
    return c * contribution;
}

float4 main(Vertex_Quad input) : SV_TARGET
{
    float3 bloom = bloomTexture.Sample(LinearWrap, input.inTexCoord).xyz;
    
    return float4(Prefilter(bloom), 0.5f) * 0.0f;
}