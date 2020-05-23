//√‚√≥ : https://github.com/aaronscherzinger/directx11_bloom/blob/master/include/resource.h
//https://catlikecoding.com/unity/tutorials/advanced-rendering/bloom/

Texture2D<float4> inputTexture : register(t0);
RWTexture2D<float4> outputTexture : register(u0);

static const float3 lumCoeff = float3(0.299f, 0.587f, 0.114f);

inline half luminance(half3 color)
{
    return max(dot(color, lumCoeff), 0.0001f);
}

cbuffer DownSampleBuffer : register(b0)
{
    float tr;
    float cnt;
    float2 radius;
}

half3 Dt(half3 c)
{
    if (length(c) > tr)
    {
        return c;
    }
    
    return 0.0f;
}

half3 DownSampleBox2(float2 pixel)
{
    half4 o = float4(radius, radius * -1);

    half3 s1 = Dt(inputTexture[pixel + o.xy].xyz);
    half3 s2 = Dt(inputTexture[pixel + o.zy].xyz);
    half3 s3 = Dt(inputTexture[pixel + o.xw].xyz);
    half3 s4 = Dt(inputTexture[pixel + o.zw].xyz);
    
    
    
    return (s1 + s2 + s3 + s4) *0.25f;
}

half3 DownSampleBox(float2 pixel)
{
    half4 o = float4(radius, radius * -1);

    half3 s1 = inputTexture[pixel + o.xy].xyz;
    half3 s2 = inputTexture[pixel + o.zy].xyz;
    half3 s3 = inputTexture[pixel + o.xw].xyz;
    half3 s4 = inputTexture[pixel + o.zw].xyz;
    
    float s1w = 1 / (luminance(s1) + 1);
    float s2w = 1 / (luminance(s2) + 1);
    float s3w = 1 / (luminance(s3) + 1);
    float s4w = 1 / (luminance(s4) + 1);
    float one_div_wsum = 1.0 / (s1w + s2w + s3w + s4w);
    
    return (s1 * s1w + s2 * s2w + s3 * s3w + s4 * s4w) * one_div_wsum;
    
    //return s * 0.25f;
}

[numthreads(8, 8, 1)]
void main(uint3 groupID : SV_GroupID, uint3 groupThreadID : SV_GroupThreadID, uint groupIndex : SV_GroupIndex, uint3 dispatchID : SV_DispatchThreadID)
{
    // output pixel in half resolution
    uint2 pixel = uint2(dispatchID.x, dispatchID.y);
    uint2 inPixel = pixel * 2;
    
    if (cnt == 0)
    {
        outputTexture[pixel] = half4(DownSampleBox2(inPixel), 1.0f);
    }
    else
    {
        outputTexture[pixel] = half4(DownSampleBox(inPixel), 1.0f);
    }
    
}