#include "Include/ObjectHeader.hlsli"

struct G2P
{
    float4 inPosition : SV_POSITION0;
    float2 inTexCoord : TEXCOORD0;
    float3 inNormal : NORMAL0;
    float3 inWorldPos : WORLD_POSITION0;
    float3 inTangent : TANGENT;
    int inFurLayer : TEXCOORD1;
};

cbuffer FurDate : register(b3)
{
    int FurLayer;
    float FurLength;
    float FurOpacityThreshold;
    float FurOpacity;
    float FurDensity;
    float FurGravity;
    float2 pad;
}

Texture2D FurShellOpacityMap : TEXTURE : register(t6);

PS_OUTPUT main(G2P input) : SV_TARGET
{
    PS_OUTPUT output;
    output.pos = float4(input.inWorldPos, 1.0f);
    output.normal = float4(input.inNormal, 1.0f);
    
    output.materialProperty = float4(
    metalMap.Sample(PointClamp, input.inTexCoord).r * MetalIntensity,
    roughnessMap.Sample(PointClamp, input.inTexCoord).r * RoughnessIntensity,
    specularMap.Sample(PointClamp, input.inTexCoord).r * SpecularIntensity,
    1.0f);
    
    float depth = input.inPosition.z / input.inPosition.w;
    output.depth = float4(depth, depth, depth, 1.0f);
    
    //if (!gShowShells)
    //    return float4(0, 0, 0, 0);
    float4 diffuseColor = albedoMap.Sample(LinearWrap, input.inTexCoord);
    float3 color_rgb = diffuseColor.rgb;
    float color_a = diffuseColor.a;
	
    //opacity
    float opacity = input.inFurLayer == 0 ? 1.0f : saturate(1.0f - FurShellOpacityMap.Sample(LinearWrap, input.inTexCoord * FurDensity).r);
    //color_a = (1 - opacity.r) * pow(1, -input.inFurLayer);
    //color_a = (1 - opacity) * (1 - pow((float) input.inFurLayer / (float) FurLayer, 1.3));
    //color_a = (1 - opacity) * (1 - (float) input.inFurLayer / (float) FurLayer);
    float furStep = FurLength / FurLayer * input.inFurLayer;
    color_a = saturate(opacity - pow(furStep, 2) * FurOpacity);
    if (color_a <= FurOpacityThreshold)
        discard;
    
    //float diffuseStrength = dot(-input.inNormal, spotLight.Forward);
    //diffuseStrength = diffuseStrength * 0.5 + 0.5;
    //diffuseStrength = saturate(diffuseStrength);
    //color_rgb = color_rgb * diffuseStrength;
		
	//increase brightness at the end of strands
    color_rgb *= 0.8f + (float) input.inFurLayer / (2 * FurLayer);
    output.color = float4(color_rgb, color_a);
    
    return output;
}