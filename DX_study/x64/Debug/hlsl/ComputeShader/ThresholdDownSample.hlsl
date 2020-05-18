//√‚√≥ : https://github.com/aaronscherzinger/directx11_bloom/blob/master/include/resource.h
//https://catlikecoding.com/unity/tutorials/advanced-rendering/bloom/

Texture2D<float4> inputTexture : register(t0);
RWTexture2D<float4> outputTexture : register(u0);

half3 DownSampleBox(float2 pixel)
{
    half4 o = float2(-1, 1).xxyy * 3;
    half3 s =
				inputTexture[pixel + o.xy].xyz + inputTexture[pixel + o.zy].xyz +
				inputTexture[pixel + o.xw].xyz + inputTexture[pixel + o.zw].xyz;
    return s * 0.25f;
}

[numthreads(8, 8, 1)]
void main(uint3 groupID : SV_GroupID, uint3 groupThreadID : SV_GroupThreadID, uint groupIndex : SV_GroupIndex, uint3 dispatchID : SV_DispatchThreadID)
{
    // output pixel in half resolution
    uint2 pixel = uint2(dispatchID.x, dispatchID.y);
    uint2 inPixel = pixel * 2;
    
    outputTexture[pixel] = half4(DownSampleBox(inPixel), 1.0f);
}