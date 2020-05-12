#ifndef PIXELSHADER_COMMONTYPES_HLSLI_
#define PIXELSHADER_COMMONTYPES_HLSLI_

struct SpotLight
{
    float3 Position;
    float Range;

    float3 Forward;
    float SpotAngle;

    float3 Color;
    float Strength;
    
    float3 Attenuation;
    float conePlaneD;
    
    float4x4 ViewProjMatrix;
};

struct Vertex_Basic
{
    float4 inPosition : SV_POSITION0;
    float2 inTexCoord : TEXCOORD0;
    float3 inNormal : NORMAL0;
    float3 inWorldPos : WORLD_POSITION0;
    float3 inTangent : TANGENT;
    int inOther : TEXCOORD1;
};

struct Vertex_Quad
{
    float4 inPosition : SV_POSITION;
    float2 inTexCoord : TEXCOORD;
};

struct Pixel_Deferred
{
    float4 pos : WORLD_POSITION;
    float4 normal : NORMAL;
    float4 color : COLOR0;
    float4 materialProperty : COLOR1;
    float4 depth : COLOR2;
};



#endif