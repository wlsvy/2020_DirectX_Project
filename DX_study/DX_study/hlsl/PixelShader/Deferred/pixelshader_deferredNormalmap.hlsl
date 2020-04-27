struct PS_INPUT
{
	float4 inPosition : SV_POSITION0;
	float2 inTexCoord : TEXCOORD0;
	float3 inNormal : NORMAL0;
	float3 inWorldPos : WORLD_POSITION0;
};

struct PS_OUTPUT
{
	float4 pos : WORLD_POSITION;
	float4 normal : NORMAL;
	float4 color : COLOR0;
	float4 light : COLOR1;
};

cbuffer lightBuffer : register(b0)
{
	float3 position;
	float range;
    
	float3 forwardVector;
	float spotAngle;
    
	float3 lightColor;
	float strength;
    
	float3 ambientColor;
	float ambientStrength;
    
	float3 attentuation;
	float pad;
    
	float4x4 lightVPmatrix;
}

cbuffer Material : register(b1)
{
	float4 materialColor;
};

Texture2D objTexture : TEXTURE : register(t0);
Texture2D normalMap : TEXTURE : register(t1);
Texture2D shadowMap : TEXTURE : register(t3);

SamplerState objSamplerState : SAMPLER : register(s0);
SamplerState objSamplerState1 : SAMPLER : register(s1);

float4 CalculateShadow(int lightIndex, float4 lightSpacePos)
{
	float lightDepth = 1 - lightSpacePos.z / range + 0.01;
    
	float2 projectTexCoord = 0.5f * lightSpacePos.xy / lightSpacePos.w + 0.5f;
	projectTexCoord.y = 1.0f - projectTexCoord.y;
        
	if (projectTexCoord.x <= 0.0f || projectTexCoord.x >= 1.0f || projectTexCoord.y <= 0.0f || projectTexCoord.y >= 1.0f || lightDepth > 1.0f)
	{
		return float4(1.0f, 1.0f, 1.0f, 1.0f);
	}
    
	float shadowMapDepth = shadowMap.Sample(objSamplerState1, projectTexCoord).r;
    
	if (lightDepth >= shadowMapDepth)
	{
		return float4(1.0f, 1.0f, 1.0f, 1.0f);
	}
	return float4(0.0f, 0.0f, 0.0f, 0.0f);
}


float4 CalculateLightColor(float3 worldPos, float3 normal)
{
	float3 vectorToLight = position - worldPos;
	float distToLight = length(vectorToLight);
	vectorToLight /= distToLight;
	float diffuseLightIntensity = max(dot(vectorToLight, normal), 0.0f);
	if (diffuseLightIntensity > 0.0f)
	{
		float attFactor = 1 / (attentuation.x + (attentuation.y * distToLight) + (attentuation.z * pow(distToLight, 2)));
		float spotFactor = pow(max(dot(-vectorToLight, forwardVector), 0.0f), 1.0f);
		return float4(lightColor, 1.0f) * strength * spotFactor * attFactor;

	}
    
	return float4(0.0f, 0.0f, 0.0f, 1.0f);
}

PS_OUTPUT main(PS_INPUT input) : SV_TARGET
{
	PS_OUTPUT output;
	output.pos = float4(input.inWorldPos, 1.0f);
	output.color = normalMap.Sample(objSamplerState, input.inTexCoord) * materialColor;
	
	float3 textureNormal = normalMap.Sample(objSamplerState, input.inTexCoord);
	textureNormal = (textureNormal * 2.0f) - 1.0f;
	textureNormal = textureNormal.z * input.inNormal;
	output.normal = float4(normalize(textureNormal), 1.0f);
	
	float4 lightSpacePos = mul(float4(input.inWorldPos, 1.0f), transpose(lightVPmatrix)); // 도대체 왜 이걸 전치시켜야 하는지 이해를 못하겠다
	output.light = CalculateShadow(0, lightSpacePos) * CalculateLightColor(input.inWorldPos.xyz, input.inNormal);
	return output;
}

/*
 float4 textureColor;
    float4 bumpMap;
    float3 bumpNormal;
    float3 lightDir;
    float lightIntensity;
    float4 color;


    // Sample the texture pixel at this location.
    textureColor = shaderTextures[0].Sample(SampleType, input.tex);
	
    // Sample the pixel in the bump map.
    bumpMap = shaderTextures[1].Sample(SampleType, input.tex);

    // Expand the range of the normal value from (0, +1) to (-1, +1).
    bumpMap = (bumpMap * 2.0f) - 1.0f;

    // Calculate the normal from the data in the bump map.
    bumpNormal = (bumpMap.x * input.tangent) + (bumpMap.y * input.binormal) + (bumpMap.z * input.normal);
	
    // Normalize the resulting bump normal.
    bumpNormal = normalize(bumpNormal);

    // Invert the light direction for calculations.
    lightDir = -lightDirection;

    // Calculate the amount of light on this pixel based on the bump map normal value.
    lightIntensity = saturate(dot(bumpNormal, lightDir));

    // Determine the final diffuse color based on the diffuse color and the amount of light intensity.
    color = saturate(diffuseColor * lightIntensity);

    // Combine the final bump light color with the texture color.
    color = color * textureColor;
	
    return color;
*/