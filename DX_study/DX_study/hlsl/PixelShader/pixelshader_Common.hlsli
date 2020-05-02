const float PI = 3.14159265358979323846;
const float INFINITY = 100000.0f;
static const float Epsilon = 0.00001;

SamplerState PointClamp : SAMPLER : register(s0);
SamplerState LinearWrap : SAMPLER : register(s1);

cbuffer CB_Scene : register(b0)
{
    float3 CameraPosition;
    float pad0;
    float3 CameraForward;
    float pad1;
    
    float ElapsedTime;
    float DeltaTime;
    float2 pad2;
    
    float3 AmbientColor;
    float AmbientStrength;
    
    float4x4 InverseViewMatrix;
    float4x4 InverseProjMatrix;
    
    float SSAO_strength;
    float SSAO_radius;
    float SSAO_bias;
    float SSAO_scale;
    
    // x: scattering coef, y: extinction coef, z: range w: skybox extinction coef
    float3 VolumetricLightVar;
    int VolumetricLightSampleCount;
}

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

cbuffer CB_SpotLight : register(b1)
{
    SpotLight spotLight;
}

float hash(float x)
{
    return frac(sin(x) * 43758.5453);
}

float3 GetRandomVector(float seed)
{
    float r2 = hash(seed);
    float phi = 2. * PI * hash(seed + r2);
    float sina = sqrt(r2);
    float cosa = sqrt(1. - r2);

    return float3(cos(phi) * sina, cosa, sin(phi) * sina);
}


//√‚√≥ : http://lousodrome.net/blog/light/2017/01/03/intersection-of-a-ray-and-a-cone/
bool IsRayPositionInsideCone(float3 rayOrigin, float3 rayDir, float t)
{
    if (t < 0)
        return false;
    float3 cp = rayOrigin + t * rayDir - spotLight.Position;
    float h = dot(cp, spotLight.Forward);
    return h > 0 && h < spotLight.Range;
}

bool2 RayConeIntersect(float3 rayOrigin, float3 rayDir, out float near, out float far)
{
    near = INFINITY;
    far = INFINITY;
    float cosa = cos(radians(spotLight.SpotAngle * 0.5));
    float3 co = rayOrigin - spotLight.Position;

    float a = dot(rayDir, spotLight.Forward) * dot(rayDir, spotLight.Forward) - cosa * cosa;
    float b = 2. * (dot(rayDir, spotLight.Forward) * dot(co, spotLight.Forward) - dot(rayDir, co) * cosa * cosa);
    float c = dot(co, spotLight.Forward) * dot(co, spotLight.Forward) - dot(co, co) * cosa * cosa;

    float det = b * b - 4. * a * c;
    if (det < 0.)
        return bool2(false, false);

    det = sqrt(det);
    float t1 = (-b - det) / (2. * a);
    float t2 = (-b + det) / (2. * a);

    bool inside1 = IsRayPositionInsideCone(rayOrigin, rayDir, t1);
    bool inside2 = IsRayPositionInsideCone(rayOrigin, rayDir, t2);

    float t = t1;
    if (!inside1 && !inside2)
        return bool2(false, false);
    if (!inside1)
    {
        near = t2;
        return bool2(true, false);
    }
    else if (!inside2)
    {
        near = t1;
        return bool2(true, false);
    }
    
    near = min(t1, t2);
    far = max(t1, t2);
    return bool2(true, true);
}

bool ContainedBySpotLight(float3 lightToPosVec, float lightToPosDist)
{
    if (lightToPosDist > spotLight.Range)
    {
        return false;
    }
    
    float cosAngle = dot(lightToPosVec, spotLight.Forward);
    
    return cosAngle > 0 && cos(radians(spotLight.SpotAngle * 0.5)) < cosAngle;
}

bool ContainedBySpotLight(float3 pos)
{
    float3 lightToPosVec = pos - spotLight.Position;
    float lightToPosDist = length(lightToPosVec);
    if (length(pos - spotLight.Position) > spotLight.Range)
    {
        return false;
    }
    
    lightToPosVec /= lightToPosDist;
    float cosAngle = dot(lightToPosVec, spotLight.Forward);
    return cosAngle > 0 && cos(radians(spotLight.SpotAngle * 0.5)) < cosAngle;
}

float4 CalculateLightColor(float3 worldPos, float3 normal)
{
    float3 vectorToLight = spotLight.Position - worldPos;
    float distToLight = length(vectorToLight);
    vectorToLight /= distToLight;
    float diffuseLightIntensity = max(dot(vectorToLight, normal), 0.0f);
    if (diffuseLightIntensity > 0.0f)
    {
        float attFactor = 1 / (spotLight.Attenuation.x + (spotLight.Attenuation.y * distToLight) + (spotLight.Attenuation.z * pow(distToLight, 2)));
        float spotFactor = pow(max(dot(-vectorToLight, spotLight.Forward), 0.0f), 1.0f);
        return float4(spotLight.Color, 1.0f) * spotLight.Strength * spotFactor * attFactor;

    }
    
    return float4(0.0f, 0.0f, 0.0f, 1.0f);
}

bool RayPlaneIntersect(in float3 planeNormal, in float3 planeOrigin, in float3 rayOrigin, in float3 rayDir, out float t)
{
    float NdotD = dot(planeNormal, rayDir);
    if (NdotD > 0.000001)
    {
        t = dot((planeOrigin - rayOrigin), planeNormal) / NdotD;
        return t >= 0;
    }
    
    return false;
}

bool RayDiskIntersect(in float3 planeNormal, in float planeD, in float radius, in float3 rayOrigin, in float3 rayDir, out float t)
{
    t = INFINITY;
    float3 planeOrigin = planeNormal * -planeD;
    if (RayPlaneIntersect(planeNormal, planeOrigin, rayOrigin, rayDir, t))
    {
        float3 intersectionPos = rayOrigin + rayDir * t;
        float3 vec = intersectionPos - planeOrigin;
        float dist = length(vec);
        return (dist <= radius);
    }
    
    return false;
}