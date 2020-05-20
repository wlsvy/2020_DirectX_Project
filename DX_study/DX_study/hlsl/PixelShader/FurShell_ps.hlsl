#include "../Include/Common.hlsli"

struct G2P
{
    float4 inPosition : SV_POSITION0;
    float2 inTexCoord : TEXCOORD0;
    float3 inNormal : NORMAL0;
    float3 inWorldPos : WORLD_POSITION0;
    float3 inTangent : TANGENT;
    int inFurLayer : TEXCOORD1;
};


Pixel_Deferred main(G2P input) : SV_TARGET
{
    //if (!gShowShells)
    //    return float4(0, 0, 0, 0);
    float4 diffuseColor = MaterialAlbedoMap.Sample(LinearWrap, input.inTexCoord);
	
    //opacity
    float opacity = input.inFurLayer == 0 ? 1.0f : saturate(1.0f - FurShellOpacityMap.Sample(LinearWrap, input.inTexCoord * FurDensity).r);
    //color_a = (1 - opacity.r) * pow(1, -input.inFurLayer);
    //color_a = (1 - opacity) * (1 - pow((float) input.inFurLayer / (float) FurLayer, 1.3));
    //color_a = (1 - opacity) * (1 - (float) input.inFurLayer / (float) FurLayer);
    float furStep = FurLength / FurLayer * input.inFurLayer;
    diffuseColor.a = saturate(opacity - pow(furStep, 2) * FurOpacity);
    if (diffuseColor.a <= FurOpacityThreshold)
        discard;
    
    //float diffuseStrength = dot(-input.inNormal, spotLight.Forward);
    //diffuseStrength = diffuseStrength * 0.5 + 0.5;
    //diffuseStrength = saturate(diffuseStrength);
    //color_rgb = color_rgb * diffuseStrength;
		
	//increase brightness at the end of strands
    diffuseColor.rgb *= 0.8f + (float) input.inFurLayer / (2 * FurLayer);
    
    Pixel_DeferredOpaque output;
    output.pos = input.inWorldPos;
    output.colorFlag = diffuseColor.a;
    output.normal = input.inNormal;
    output.depth = input.inPosition.z / input.inPosition.w;
    output.color = diffuseColor.rgb * AlbedoColor.rgb;
    output.metal = MaterialMetalMap.Sample(PointClamp, input.inTexCoord).r * MetalIntensity;
    output.emission = MaterialEmissionMap.Sample(PointClamp, input.inTexCoord).r * EmissionIntensity;
    output.roughness = MaterialRoughMap.Sample(PointClamp, input.inTexCoord).r * RoughnessIntensity;
    return output;
}