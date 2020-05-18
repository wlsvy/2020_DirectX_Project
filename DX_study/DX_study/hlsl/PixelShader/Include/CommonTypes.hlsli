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

struct Pixel_DeferredOpaque
{
    //Render Target 0
    float3 pos : WORLD_POSITION;
    float depth : DEPTH;
    
    //Render Target 1
    float3 color : COLOR0;
    float colorFlag : FLAG; // -1 : skybox, 0 ~ 1 : objectTransparency
    
    //Render Target 2
    float3 normal : NORMAL;
    float metal : METAL;
    
    //Render Target 3
    float emission : EMISSION;
    float roughness : ROUGHNESS;
    float2 pad;
};

struct Pixel_Deferred
{
    float4 output0;
    float4 output1;
    float4 output2;
    float4 output3;
};

#endif