cbuffer perObjectBuffer : register(b0)
{
    float4x4 wvpMatrix;
    float4x4 worldMatrix;
    float4x4 vpMatrix;
};

cbuffer FurData : register(b1)
{
    int FurLayer;
    float FurLength;
    float FurOpacityThreshold;
    float FurOpacity;
    float FurDensity;
    float FurGravity;
    float2 pad;
};

struct GS_INPUT
{
    float3 inPosition : POSITION;
    float2 inTexCoord : TEXCOORD;
    float3 inNormal : NORMAL;
    float4 inTangent : TANGENT;
};

struct GS_OUTPUT
{
    float4 outPosition : SV_POSITION0;
    float2 outTexCoord : TEXCOORD0;
    float3 outNormal : NORMAL0;
    float3 outWorldPos : WORLD_POSITION0;
    float3 outTangent : TANGENT;
    int outFurLayer : TEXCOORD1;
};

void CreateVertex(inout TriangleStream<GS_OUTPUT> triStream, float3 pos, float3 normal, float2 texCoord, float3 tangent, int layerIndex)
{
    GS_OUTPUT output;
    
    float4x4 transposedWmat = transpose(worldMatrix);
    
    output.outPosition = mul(float4(pos, 1.0f), transpose(wvpMatrix));
    output.outNormal = mul(float4(normal, 1.0f), transposedWmat);
    output.outWorldPos = mul(float4(pos, 1.0f), transposedWmat);
    output.outTangent = mul(float4(tangent, 1.0f), transposedWmat);
    output.outTexCoord = texCoord;
    output.outFurLayer = layerIndex;
		
    triStream.Append(output);
}

//SHELLS
//******
//amount of layers * 3 vertices for a triangle
[maxvertexcount(60)]
void main(triangle GS_INPUT vertices[3], inout TriangleStream<GS_OUTPUT> triStream)
{
    float offset = FurLength / FurLayer * 0.01f;
	
    CreateVertex(triStream, vertices[0].inPosition, vertices[1].inNormal, vertices[1].inTexCoord, vertices[1].inTangent.xyz, 0);
    CreateVertex(triStream, vertices[1].inPosition, vertices[2].inNormal, vertices[2].inTexCoord, vertices[2].inTangent.xyz, 0);
    CreateVertex(triStream, vertices[2].inPosition, vertices[0].inNormal, vertices[0].inTexCoord, vertices[0].inTangent.xyz, 0);
    triStream.RestartStrip();
    
	[loop]
    for (float i = 1; i <= FurLayer; ++i)
    {
        float g = pow((float) i / FurLayer, 3) * FurGravity;
        
        float3 pos0 = vertices[0].inPosition + vertices[0].inNormal * offset * i + float3(0.0f, g * -1.0f, 0.0f);
        float3 pos1 = vertices[1].inPosition + vertices[1].inNormal * offset * i + float3(0.0f, g * -1.0f, 0.0f);
        float3 pos2 = vertices[2].inPosition + vertices[2].inNormal * offset * i + float3(0.0f, g * -1.0f, 0.0f);

        CreateVertex(triStream, pos1, vertices[1].inNormal, vertices[1].inTexCoord, vertices[1].inTangent.xyz, i);
        CreateVertex(triStream, pos2, vertices[2].inNormal, vertices[2].inTexCoord, vertices[2].inTangent.xyz, i);
        CreateVertex(triStream, pos0, vertices[0].inNormal, vertices[0].inTexCoord, vertices[0].inTangent.xyz, i);

        triStream.RestartStrip();
    }
}