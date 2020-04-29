//√‚√≥ : https://github.com/aaronscherzinger/directx11_bloom/blob/master/include/resource.h
#define GAUSSIAN_RADIUS 7

Texture2D<float4> inputTexture : register(t0);
RWTexture2D<float4> outputTexture : register(u0);

cbuffer BlurParams : register(b0)
{
    // = float coefficients[GAUSSIAN_RADIUS + 1]
    float4 coefficients[(GAUSSIAN_RADIUS + 1) / 4];
    int radius; // must be <= MAX_GAUSSIAN_RADIUS
    int direction; // 0 = horizontal, 1 = vertical
    float threshold;
    float pad;
}

[numthreads(8, 8, 1)]
void main(uint3 groupID : SV_GroupID, uint3 groupThreadID : SV_GroupThreadID, uint groupIndex : SV_GroupIndex, uint3 dispatchID : SV_DispatchThreadID)
{
    int2 pixel = int2(dispatchID.x, dispatchID.y);

    int2 dir = int2(1 - direction, direction);

    float4 accumulatedValue = float4(0.0, 0.0, 0.0, 0.0);

    for (int i = -radius; i <= radius; ++i)
    {
        uint cIndex = (uint) abs(i);
        accumulatedValue += coefficients[cIndex >> 2][cIndex & 3] * inputTexture[mad(i, dir, pixel)];
    }

    outputTexture[pixel] = accumulatedValue;
}

//float4 main(float2 texCoord : TEXCOORD0) : COLOR0
//{ 
//    // Calculate vector from pixel to light source in screen space.    
//    half2 deltaTexCoord = (texCoord - ScreenLightPos.xy);   
//    // Divide by number of samples and scale by control factor.  
//    deltaTexCoord *= 1.0f / NUM_SAMPLES * Density;   
//    // Store initial sample.    
//    half3 color = tex2D(frameSampler, texCoord);   
//    // Set up illumination decay factor.    
//    half illuminationDecay = 1.0f;   
//    // Evaluate summation from Equation 3 NUM_SAMPLES iterations.    
//        for (int i = 0; i < NUM_SAMPLES; i++)   {     
//        // Step sample location along ray.     
//        texCoord -= deltaTexCoord;     
//        // Retrieve sample at new location.    
//        half3 sample = tex2D(frameSampler, texCoord);     
//        // Apply sample attenuation scale/decay factors.     
//        sample *= illuminationDecay * Weight;     // Accumulate combined color.     
//        color += sample;     // Update exponential decay factor.     
//        illuminationDecay *= Decay;   
//        }   // Output final color with a further scale control factor.    
//    return float4( color * Exposure, 1); 
//    } 
