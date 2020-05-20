#include "../Include/Common.hlsli"

float4 main(Vertex_Quad input) : SV_TARGET
{
    float4 color = CompositionTexture.Sample(LinearWrap, input.inTexCoord);
    return float4(pow(color.xyz, 1.0 / Gamma), 1.0f);
}