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

TextureCube SkyBoxCube : TEXTURE_CUBE : register(t1);

float4 main(PS_INPUT input) : SV_TARGET
{
    return SkyBoxCube.Sample(samTriLinearSam, input.inLocalPos);
}

