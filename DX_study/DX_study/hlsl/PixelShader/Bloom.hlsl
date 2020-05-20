#include "../Include/Common.hlsli"

half3 Prefilter(half3 c)
{
    half brightness = max(c.r, max(c.g, c.b));
    if (brightness > Bloom_Threshold)
    {
        return c;
    }
    return half3(0.0f, 0.0f, 0.f);
    
    
    half contribution = max(0, brightness - Bloom_Threshold);
    contribution /= max(brightness, 0.00001);
    return c * contribution;
}

float4 main(Vertex_Quad input) : SV_TARGET
{
    float3 bloom = BloomTexture.Sample(LinearWrap, input.inTexCoord).xyz;
    
    half3 filtered = Prefilter(bloom);
    
    return float4(filtered, length(filtered)) * Bloom_Intensity;
}