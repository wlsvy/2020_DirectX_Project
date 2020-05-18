#include "Include/Common.hlsli"

struct PS_INPUT
{
    float4 inPosition : SV_POSITION;
    float3 inLocalPos : LOCAL_POSITION;
};

struct PS_OUTPUT
{
    float4 pos : WORLD_POSITION;
    float4 normal : NORMAL;
    float4 color : COLOR;
    float4 light : COLOR1;
    float4 depth : COLOR2;
};

Pixel_Deferred main(PS_INPUT input) : SV_TARGET
{
    Pixel_DeferredOpaque output;
    output.pos = input.inLocalPos * 1000;
    output.colorFlag = -1.0f;
    output.normal = float3(-1.0f, -1.0f, -1.0f);
    output.depth = 1.0f;
    output.color = SkyboxCubeMap.Sample(LinearWrap, input.inLocalPos);
    output.metal = 1.0f;
    output.specular = float3(1.0f, 1.0f, 1.0f);
    output.roughness = 1.0f;
    return output;
}

