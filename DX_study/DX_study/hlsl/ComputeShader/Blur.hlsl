//√‚√≥ : https://github.com/aaronscherzinger/directx11_bloom/blob/master/include/resource.h
#define GAUSSIAN_RADIUS 7

Texture2D<float4> inputTexture : register(t0);
RWTexture2D<float4> outputTexture : register(u0);

cbuffer BlurParams : register(b0)
{
    // = float coefficients[GAUSSIAN_RADIUS + 1]
    float4 coefficients[(GAUSSIAN_RADIUS + 1) / 4];
    int radius; // must be <= MAX_GAUSSIAN_RADIUS
    int direction; // 0 = horizontal, 1 = vertical
    float threshold;
    float pad;
}

[numthreads(8, 8, 1)]
void main(uint3 groupID : SV_GroupID, uint3 groupThreadID : SV_GroupThreadID, uint groupIndex : SV_GroupIndex, uint3 dispatchID : SV_DispatchThreadID)
{
    int2 pixel = int2(dispatchID.x, dispatchID.y);

    int2 dir = int2(1 - direction, direction);

    float4 accumulatedValue = float4(0.0, 0.0, 0.0, 0.0);

    for (int i = -radius; i <= radius; ++i)
    {
        uint cIndex = (uint) abs(i);
        accumulatedValue += coefficients[cIndex >> 2][cIndex & 3] * inputTexture[mad(i, dir, pixel)];
    }

    outputTexture[pixel] = accumulatedValue;
}