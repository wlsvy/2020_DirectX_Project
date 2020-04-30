#pragma pack_matrix(row_major)
#define MAX_BONE_NUM 100

cbuffer perObjectBuffer : register(b0)
{
    float4x4 wvpMatrix;
    float4x4 worldMatrix;
};

cbuffer boneBuffer : register(b1)
{
    float4x4 boneTransform[MAX_BONE_NUM];
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
    int4 inBoneIDs : BONEID;
    float4 inBoneWeights : BONEWEIGHT;
};

struct VS_OUTPUT
{
    float4 outPosition : SV_Position;
    float2 outTexCoord : TEXCOORD0;
    float3 outNormal : NORMAL0;
    float3 outWorldPos : WORLD_POSITION0;
    float3 outTangent : TANGENT;
    float outDepth : DEPTH;
};

VS_OUTPUT main(VS_INPUT input)
{
    VS_OUTPUT output;

    float4x4 boneMatrix = boneTransform[input.inBoneIDs[0]] * input.inBoneWeights[0];
    boneMatrix += boneTransform[input.inBoneIDs[1]] * input.inBoneWeights[1];
    boneMatrix += boneTransform[input.inBoneIDs[2]] * input.inBoneWeights[2];
    boneMatrix += boneTransform[input.inBoneIDs[3]] * input.inBoneWeights[3];;

    float4x4 animatedWvpMatrix = mul(boneMatrix, wvpMatrix);
    float4x4 animatedWorldMatrix = mul(boneMatrix, worldMatrix);

    output.outPosition = mul(float4(input.inPos, 1.0f), animatedWvpMatrix);
    output.outTexCoord = input.inTexCoord;
    output.outNormal = normalize(mul(float4(input.inNormal, 0.0f), animatedWorldMatrix));
    output.outWorldPos = mul(float4(input.inPos, 1.0f), animatedWorldMatrix);
    output.outTangent = normalize(mul(input.inTangent, animatedWorldMatrix));
    output.outDepth = mul(output.outPosition, InverseProjMatrix).z;
    return output;
}