struct PS_INPUT
{
    float4 inPosition : SV_POSITION;
    float2 inTexCoord : TEXCOORD;
    float3 inNormal : NORMAL;
    float3 inWorldPos : WORLD_POSITION;
};

struct PS_OUTPUT
{
    float4 pos : WORLD_POSITION;
    float4 normal : NORMAL;
    float4 color : COLOR;
};

Texture2D objTexture : TEXTURE : register(t0);
SamplerState objSamplerState : SAMPLER : register(s0);

PS_OUTPUT main(PS_INPUT input) : SV_TARGET
{
    PS_OUTPUT output;
    output.pos = float4(input.inWorldPos, 1.0f);
    output.normal = float4(input.inNormal, 1.0f);
    output.color = objTexture.Sample(objSamplerState, input.inTexCoord);
    return output;
}