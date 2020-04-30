#pragma pack_matrix(row_major)

cbuffer perObjectBuffer : register(b0)
{
    float4x4 wvpMatrix;
    float4x4 worldMatrix;
    float4x4 vpMatrix;
};

cbuffer CB_Scene : register(b2)
{
    float3 CameraPosition;
    float pad0;
    
    float3 AmbientColor;
    float AmbientStrength;
    
    float4x4 InverseViewMatrix;
    float4x4 InverseProjMatrix;
    
    float SSAO_strength;
    float SSAO_radius;
    float SSAO_bias;
    float SSAO_scale;
    
    // x: scattering coef, y: extinction coef, z: range w: skybox extinction coef
    float4 _VolumetricLight;
     // x: 1 - g^2, y: 1 + g^2, z: 2*g, w: 1/4pi
    float4 _MieG;
    float3 CameraForward;
    int _SampleCount;
    // x:  ground level, y: height scale, z: unused, w: unused
    float4 _HeightFog;
}

struct VS_INPUT
{
    float3 inPos : POSITION;
    float2 inTexCoord : TEXCOORD;
    float3 inNormal : NORMAL;
    float4 inTangent : TANGENT0;
};

struct VS_OUTPUT
{
    float4 outPosition : SV_Position;
    float2 outTexCoord : TEXCOORD0;
    float3 outNormal : NORMAL;
    float3 outWorldPos : WORLD_POSITION;
    float3 outTangent : TANGENT;
    float outDepth : DEPTH;
};

VS_OUTPUT main(VS_INPUT input)
{
    VS_OUTPUT output;
    output.outPosition = mul(float4(input.inPos, 1.0f), wvpMatrix);
    output.outTexCoord = input.inTexCoord;
    output.outNormal = normalize(mul(float4(input.inNormal, 0.0f), worldMatrix));
    //output.outWorldPos = mul(float4(input.inPos, 1.0f), worldMatrix);
    
    float4x4 inverseVP = mul(InverseProjMatrix, InverseViewMatrix);
    float4 viewPos = mul(output.outPosition, InverseProjMatrix);
    //viewPos /= viewPos.w;
    float4 wpos = mul(output.outPosition, inverseVP);
    //float4 wpos = mul(viewPos, InverseViewMatrix);

    //float4 wpos = mul(mul(float4(input.inPos, 1.0f), InverseProjMatrix), InverseViewMatrix);

    float4x4 wmat = mul(wvpMatrix, inverseVP);
    output.outWorldPos = wpos;
    //output.outWorldPos = mul(float4(input.inPos, 1.0f), wmat);
    
    output.outTangent = normalize(mul(input.inTangent, worldMatrix));
    output.outDepth = mul(output.outPosition, InverseProjMatrix).z;

    return output;
}