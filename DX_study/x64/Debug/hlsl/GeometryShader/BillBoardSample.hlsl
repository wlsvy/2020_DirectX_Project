cbuffer perObjectBuffer : register(b0)
{
    float4x4 wvpMatrix;
    float4x4 worldMatrix;
    float4x4 vpMatrix;
};

cbuffer cameraBuffer : register(b1)
{
    float3 dynamicCameraPosition;
    float b1pad;
}

struct VertexIn
{
    float2 inTexCoord : TEXCOORD;
    float3 inNormal : NORMAL;
    float3 inWorldPos : WORLD_POSITION;
    float4 outTestPosition : TEST_POSITION;
};

struct GeoOut
{
    float4 outPosition : SV_POSITION;
    float2 outTexCoord : TEXCOORD;
    float3 outNormal : NORMAL;
    float3 outWorldPos : WORLD_POSITION;
};

//[maxvertexcount(1)]
//void main(point VertexIn vertin[1], inout PointStream<GeoOut> output)
//{
//    GeoOut outputVert;

//    outputVert.outPosition = vertin[0].inPosition;
//    outputVert.outWorldPos = vertin[0].inWorldPos;
//    outputVert.outTexCoord = vertin[0].inTexCoord;
//    outputVert.outNormal = vertin[0].inNormal;

//    output.Append(outputVert);
//}
[maxvertexcount(2)]
void main4(point VertexIn vertin[1], inout PointStream<GeoOut> output)
{
    GeoOut outputvert;
    outputvert.outNormal = vertin[0].inNormal;
    outputvert.outPosition = mul(vpMatrix, float4(vertin[0].inWorldPos, 1.0f));
    outputvert.outTexCoord = vertin[0].inTexCoord;
    outputvert.outWorldPos = vertin[0].inWorldPos;

    output.Append(outputvert);
}

[maxvertexcount(4)]
void main(point VertexIn vertin[1], inout TriangleStream<GeoOut> output){
    float halfWidth = 5.0f;
    float halfHeight = 5.0f;

    float3 planeNormal = vertin[0].inWorldPos - dynamicCameraPosition;
    planeNormal.y = 0.0f;
    planeNormal = normalize(planeNormal);

    float3 upVector = float3(0.0f, 1.0f, 0.0f);

    // Now we need to find the billboards right vector, so we can easily find the billboards vertices from the input point
    float3 rightVector = normalize(cross(planeNormal, upVector)); // Cross planes normal with the up vector (+y) to get billboards right vector
    //float3 rightVector = float3(1.0f, 0.0f, 0.0f);
    rightVector = rightVector * halfWidth; // change the length of the right vector to be half the width of the billboard

    // Get the billboards "height" vector which will be used to find the top two vertices in the billboard quad
    upVector = float3(0, halfHeight, 0);

    // Create the billboards quad
    float3 vert[4];

    // We get the points by using the billboards right vector and the billboards height
    //vert[0] =  -rightVector - upVector;
    //vert[1] =  rightVector - upVector;
    //vert[2] =  -rightVector + upVector;
    //vert[3] =  rightVector + upVector;
    //vert[0] = float3(30.0f, -30.0f, 0.0f);
    //vert[1] = float3(3.0f, 3.0f, 0.0f);
    //vert[2] = float3(-30.0f, -30.0f, 0.0f);
    //vert[3] = float3(-30.0f, 30.0f, 0.0f);
    vert[0] = vertin[0].inWorldPos - rightVector - upVector; // Get bottom left vertex
    vert[1] = vertin[0].inWorldPos - rightVector + upVector; // Get bottom right vertex
    vert[2] = vertin[0].inWorldPos + rightVector - upVector; // Get top left vertex
    vert[3] = vertin[0].inWorldPos + rightVector + upVector; // Get top right vertex

    // Get billboards texture coordinates
    float2 texCoord[4];
    texCoord[0] = float2(0, 1);
    texCoord[1] = float2(1, 1);
    texCoord[2] = float2(0, 0);
    texCoord[3] = float2(1, 0);

    // Now we "append" or add the vertices to the outgoing stream list
    GeoOut outputVert;
    [unroll]
    for (int i = 0; i < 4; i++)
    {
        //outputVert.outPosition = mul(float4(vert[i], 1.0f), vpMatrix);
        //outputVert.outPosition = mul(vpMatrix, float4(vertin[i].inWorldPos, 1.0f));
        //outputVert.outPosition = mul(vertin[i].inPosition, 10.5f);
        //outputVert.outPosition = mul(vertin[i].inPosition, vpMatrix);
        outputVert.outPosition = mul(vpMatrix, float4(vert[i], 1.0f));
        outputVert.outWorldPos = float4(vert[i], 1.0f);
        //outputVert.outWorldPos = vertin[i].inWorldPos;
        //outputVert.outPosition = mul(float4(vert[i], 1.0f), wvpMatrix);
        //outputVert.outWorldPos = vertin[i].inWorldPos;

        outputVert.outTexCoord = texCoord[i];
        outputVert.outNormal = vertin[0].inNormal;

        output.Append(outputVert);
    }
    output.RestartStrip();
}

//[maxvertexcount(8)]
//void main3(triangle VertexIn vertin[3], inout TriangleStream<GeoOut> output)
//{
//    float halfWidth = 100.0f;
//    float halfHeight = 100.0f;

//    float3 planeNormal = (vertin[0].inWorldPos - dynamicCameraPosition) / 5;
//    planeNormal.y = 0.0f;
//    planeNormal = normalize(planeNormal);

//    float3 upVector = float3(0.0f, 1.0f, 0.0f);

//    // Now we need to find the billboards right vector, so we can easily find the billboards vertices from the input point
//    //float3 rightVector = normalize(cross(planeNormal, upVector)); // Cross planes normal with the up vector (+y) to get billboards right vector
//    float3 rightVector = float3(1.0f, 0.0f, 0.0f);
//    rightVector = rightVector * halfWidth; // change the length of the right vector to be half the width of the billboard

//    // Get the billboards "height" vector which will be used to find the top two vertices in the billboard quad
//    upVector = float3(0, halfHeight, 0);

//    // Create the billboards quad
//    float3 vert[4];

//    // We get the points by using the billboards right vector and the billboards height
//    //vert[0] =  -rightVector - upVector;
//    //vert[1] =  rightVector - upVector;
//    //vert[2] =  -rightVector + upVector;
//    //vert[3] =  rightVector + upVector;
//    vert[0] = float3(30.0f, -30.0f, 0.0f);
//    vert[1] = float3(30.0f, 30.0f, 0.0f);
//    vert[2] = float3(-30.0f, -30.0f, 0.0f);
//    vert[3] = float3(-30.0f, 30.0f, 0.0f);
//    //vert[0] = vertin[0].inWorldPos - rightVector - upVector; // Get bottom left vertex
//    //vert[1] = vertin[0].inWorldPos - rightVector + upVector; // Get bottom right vertex
//    //vert[2] = vertin[0].inWorldPos + rightVector - upVector; // Get top left vertex
//    //vert[3] = vertin[0].inWorldPos + rightVector + upVector; // Get top right vertex

//    // Get billboards texture coordinates
//    float2 texCoord[4];
//    texCoord[0] = float2(0, 1);
//    texCoord[1] = float2(1, 1);
//    texCoord[2] = float2(0, 0);
//    texCoord[3] = float2(1, 0);

//    // Now we "append" or add the vertices to the outgoing stream list
//    GeoOut outputVert;
//    [unroll]
//    for (int i = 0; i < 3; i++)
//    {
//        outputVert.outPosition = mul(float4(vert[i], 1.0f), vpMatrix);
//        outputVert.outWorldPos = float4(vert[i], 1.0f);
//        //outputVert.outPosition = mul(float4(vertin[i].inLocalPos, 1.0f), wvpMatrix);
//        outputVert.outPosition = vertin[i].inPosition;
//        //outputVert.outWorldPos = vertin[i].inWorldPos;

//        outputVert.outTexCoord = texCoord[i];

//        // These will not be used for billboards
//        outputVert.outNormal = (vertin[i].inWorldPos - dynamicCameraPosition);
//        //outputVert.tangent = float3(0, 0, 0);

//        output.Append(outputVert);
//    }
//}