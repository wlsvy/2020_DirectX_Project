static const float PI = 3.14159265358979323846;

SamplerState PointClamp : SAMPLER : register(s0);
SamplerState LinearWrap : SAMPLER : register(s1);

cbuffer CB_Scene : register(b0)
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

struct Cone
{
    float cosa; // half cone angle
    float h; // height
    float3 c; // tip position
    float3 v; // axis
};

struct Plane
{
    float d; // solution to dot(n,p)+d=0
    float3 n; // normal
};

struct Ray
{
    float3 o; // origin
    float3 d; // direction
};

struct RayHit
{
    float t; // solution to p=o+t*d
    float3 n; // normal
};
RayHit MakeRayHit(float t, float3 n)
{
    RayHit rh;
    rh.t = t;
    rh.n = n;
    return rh;
}

//const RayHit noHit = MakeRayHit(-1, float3(0.0f, 0.0f, 0.0f));

const float INFINITY = 100000.0f;

bool RayCone2(Cone s, Ray r, float t)
{
    if (t < 0)
        return false;
    float3 cp = r.o + t * r.d - s.c;
    float h = dot(cp, s.v);
    return h > 0 && h < s.h;
}

bool2 RayConeIntersect(Cone s, Ray r, out float near, out float far)
{
    near = INFINITY;
    far = INFINITY;
    float3 co = r.o - s.c;

    float a = dot(r.d, s.v) * dot(r.d, s.v) - s.cosa * s.cosa;
    float b = 2. * (dot(r.d, s.v) * dot(co, s.v) - dot(r.d, co) * s.cosa * s.cosa);
    float c = dot(co, s.v) * dot(co, s.v) - dot(co, co) * s.cosa * s.cosa;

    float det = b * b - 4. * a * c;
    if (det < 0.)
        return bool2(false, false);

    det = sqrt(det);
    float t1 = (-b - det) / (2. * a);
    float t2 = (-b + det) / (2. * a);

    bool check1 = RayCone2(s, r, t1);
    bool check2 = RayCone2(s, r, t2);
    // This is a bit messy; there ought to be a more elegant solution.
    float t = t1;
    if (!check1 && !check2)
        return bool2(false, false);
    if (!check1)
    {
        near = t2;
        return bool2(true, false);
    }
    else if (!check2)
    {
        near = t1;
        return bool2(true, false);
    }
    
    near = min(t1, t2);
    far = max(t1, t2);
    return bool2(true, true);
}

bool ContainedByCone(float3 pos)
{
    if (length(pos - spotLight.Position) > spotLight.Range)
    {
        return false;
    }
    float a = dot(normalize(pos - spotLight.Position), spotLight.Forward);
    if (a < 0 || cos(radians(spotLight.SpotAngle / 2)) > a)
        return false;
    return true;
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

//ÃâÃ³ : https://github.com/SlightlyMad/VolumetricLights/blob/master/Assets/Shaders/VolumetricLight.shader#L516

float2 RayConeIntersect(in float3 f3ConeApex, in float3 f3ConeAxis, in float fCosAngle, in float3 f3RayStart, in float3 f3RayDir)
{
    float inf = 10000;
    f3RayStart -= f3ConeApex;
    float a = dot(f3RayDir, f3ConeAxis);
    float b = dot(f3RayDir, f3RayDir);
    float c = dot(f3RayStart, f3ConeAxis);
    float d = dot(f3RayStart, f3RayDir);
    float e = dot(f3RayStart, f3RayStart);
    fCosAngle *= fCosAngle;
    float A = a * a - b * fCosAngle;
    float B = 2 * (c * a - d * fCosAngle);
    float C = c * c - e * fCosAngle;
    float D = B * B - 4 * A * C;

    if (D > 0)
    {
        D = sqrt(D);
        float2 t = (-B + sign(A) * float2(-D, +D)) / (2 * A);
        bool2 b2IsCorrect = c + a * t > 0 && t > 0;
        t = t * b2IsCorrect + !b2IsCorrect * (inf);
        return t;
    }
    else // no intersection
        return inf;
}


float RayPlaneIntersect(in float3 planeNormal, in float planeD, in float3 rayOrigin, in float3 rayDir)
{
    float NdotD = dot(planeNormal, rayDir);
    float NdotO = dot(planeNormal, rayOrigin);

    float t = -(NdotO + planeD) / NdotD;
    if (t < 0)
        t = 100000;
    return t;
}

bool2 RayConeIntersect(in float3 f3ConeApex, in float3 f3ConeAxis, in float fCosAngle, in float3 f3RayStart, in float3 f3RayDir, out float2 t)
{
    t = float2(INFINITY, INFINITY);
    
    f3RayStart -= f3ConeApex;
    float a = dot(f3RayDir, f3ConeAxis);
    float b = dot(f3RayDir, f3RayDir);
    float c = dot(f3RayStart, f3ConeAxis);
    float d = dot(f3RayStart, f3RayDir);
    float e = dot(f3RayStart, f3RayStart);
    fCosAngle *= fCosAngle;
    float A = a * a - b * fCosAngle;
    float B = 2 * (c * a - d * fCosAngle);
    float C = c * c - e * fCosAngle;
    float D = B * B - 4 * A * C;

    if (D > 0)
    {
        D = sqrt(D);
        t = (-B + sign(A) * float2(-D, +D)) / (2 * A);
        bool2 b2IsCorrect = c + a * t > 0 && t > 0;
        t = t * b2IsCorrect + !b2IsCorrect * (INFINITY);
        return b2IsCorrect;
    }
    
    
    return bool2(false, false);
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


void GetConeIntersectionLength(inout float3 rayStart, float3 rayDir, inout float3 rayEnd, inout float rayLength)
{
    float diskRad = tan(radians(spotLight.SpotAngle * 0.5)) * spotLight.Range;
    
    float diskT;
    bool dResult = RayDiskIntersect(spotLight.Forward, spotLight.conePlaneD, diskRad, rayStart, rayDir, diskT);
    
    //float planeCoord;
    //bool pResult = RayPlaneIntersect(spotLight.Forward, spotLight.conePlaneD, rayStart, rayDir, planeCoord);
    
    //// ray cone intersection
    float2 coneCoords;
    bool2 cResult = RayConeIntersect(spotLight.Position, spotLight.Forward, radians(spotLight.SpotAngle * 0.5), rayStart, rayDir, coneCoords);
    
    //float z = (projectedDepth - rayLength);
    //rayLength = min(rayLength, min(planeCoord, min(lineCoords.x, lineCoords.y)));
    float minRay = min(diskT, min(coneCoords.x, coneCoords.y));
    float maxRay = max(max(dResult * diskT, max(coneCoords.x * cResult.x, coneCoords.y * cResult.y)), rayLength);
    
    //rayLength = minRay;

    rayLength = min(maxRay - minRay, rayLength - minRay);
    //rayStart += rayDir * minRay;
    rayStart += rayDir * minRay;
    rayEnd = rayStart + rayDir * rayLength;
}