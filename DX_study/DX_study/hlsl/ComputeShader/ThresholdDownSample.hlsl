//��ó : https://github.com/aaronscherzinger/directx11_bloom/blob/master/include/resource.h
Texture2D<float4> inputTexture : register(t0);
RWTexture2D<float4> outputTexture : register(u0);

#define GAUSSIAN_RADIUS  7

cbuffer ThresholdParams : register(b0)
{
    float coefficients[GAUSSIAN_RADIUS + 1];
    int radius; // must be <= MAX_GAUSSIAN_RADIUS
    int direction; // 0 = horizontal, 1 = vertical
    float threshold;
    float pad;
}

[numthreads(8, 8, 1)]
void main(uint3 groupID : SV_GroupID, uint3 groupThreadID : SV_GroupThreadID, uint groupIndex : SV_GroupIndex, uint3 dispatchID : SV_DispatchThreadID)
{
    // output pixel in half resolution
    uint2 pixel = uint2(dispatchID.x, dispatchID.y);

    // bilinear interpolation for downsampling
    uint2 inPixel = pixel * 2;
    float4 hIntensity0 = lerp(inputTexture[inPixel], inputTexture[inPixel + uint2(1, 0)], 0.5);
    float4 hIntensity1 = lerp(inputTexture[inPixel + uint2(0, 1)], inputTexture[inPixel + uint2(1, 1)], 0.5);
    float4 intensity = lerp(hIntensity0, hIntensity1, 0.5);

    // thresholding on downsampled value
    float intensityTest = (float) (length(intensity.rgb) > threshold);

    outputTexture[pixel] = float4(intensityTest * intensity.rgb, 1.0);
}