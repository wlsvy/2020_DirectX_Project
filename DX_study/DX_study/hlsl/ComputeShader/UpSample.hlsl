
Texture2D<float4> inputTexture : register(t0);
RWTexture2D<float4> outputTexture : register(u0);

SamplerState defaultSampler
{
    Filter = MIN_MAG_MIP_LINEAR;
    MaxAnisotropy = 1;
    AddressU = Wrap;
    AddressV = Wrap;
    AddressM = Wrap;
};

half3 Upsample_Box(uint2 pixel)
{
    int4 pixelDelta = int2(2, 2).xyxy * int4(-1, -1, 1, 1.);
    
    half3 upsampled =
    inputTexture[pixel + pixelDelta.xy].xyz +
    inputTexture[pixel + pixelDelta.zy].xyz +
    inputTexture[pixel + pixelDelta.xw].xyz +
    inputTexture[pixel + pixelDelta.zw].xyz;
    
    return upsampled * 0.25f;
}

[numthreads(8, 8, 1)]
void main(uint3 groupID : SV_GroupID, uint3 groupThreadID : SV_GroupThreadID, uint groupIndex : SV_GroupIndex, uint3 dispatchID : SV_DispatchThreadID)
{
    // output pixel in half resolution
    uint2 pixel = uint2(dispatchID.x, dispatchID.y);
    uint2 inPixel = pixel * 0.5;
    
    outputTexture[pixel] = half4(Upsample_Box(inPixel), 1.0f);
}