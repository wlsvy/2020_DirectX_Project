cbuffer lightBuffer : register(b0)
{
    float3 position;
    float range;
    
    float3 forwardVector;
    float spotAngle;
    
    float3 color;
    float strength;
    
    float3 ambientColor;
    float ambientStrength;
    
    float3 attentuation;
    float pad;
    
    float4x4 lightVPmatrix;
}

struct PS_INPUT
{
    float4 inPosition : SV_POSITION0;
    float2 inTexCoord : TEXCOORD0;
    float3 inNormal : NORMAL0;
    float3 inWorldPos : WORLD_POSITION0;
    float4 inTangent : TANGENT;
};

float4 main(PS_INPUT input) : SV_TARGET
{
    float4 newPos = mul(float4(input.inWorldPos, 1.0f), transpose(lightVPmatrix));
    float depth = 1 - newPos.z / range;
    float4 output = float4(depth, depth, depth, 1.0f);
    
    return output;
}