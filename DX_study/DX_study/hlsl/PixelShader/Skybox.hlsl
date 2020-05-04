SamplerState samTriLinearSam
{
    Filter = MIN_MAG_MIP_LINEAR;
    AddressU = Wrap;
    AddressV = Wrap;
};

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

TextureCube SkyBoxCube : TEXTURE_CUBE : register(t1);

PS_OUTPUT main(PS_INPUT input) : SV_TARGET
{
    PS_OUTPUT output;
    output.pos = float4(input.inLocalPos * -1000, -1.0f);
    output.normal = float4(-1.0f, -1.0f, -1.0f, -1.0f);
    output.color = SkyBoxCube.Sample(samTriLinearSam, input.inLocalPos);
    output.depth = float4(0.0f, 0.0f, 0.0f, 1.0f);
    return output;
}

