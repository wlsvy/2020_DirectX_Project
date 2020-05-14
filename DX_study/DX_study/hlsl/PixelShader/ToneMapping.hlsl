#include "Include/Common.hlsli"

float3 Reinhard(float3 hdr, float k = 1.0f)
{
    return hdr / (hdr + k);
}

static const float A = 0.15;
static const float B = 0.50;
static const float C = 0.10;
static const float D = 0.20;
static const float E = 0.02;
static const float F = 0.30;
static const float W = 11.2;

float3 Uncharted2(float3 x)
{
    return ((x * (A * x + C * B) + D * E) / (x * (A * x + B) + D * F)) - E / F;
}

float4 main(Vertex_Quad input) : SV_TARGET
{
    float4 color = CompositionTexture.Sample(LinearWrap, input.inTexCoord);
    
    //return float4(Reinhard(color.xyz * exp(HdrExposure)), 1.0f);
    //return float4(Uncharted2(color.xyz * exp(HdrExposure)), 1.0f);
    return 1.0 - exp(-color * HdrExposure);
}