#include "BaseGeometry.h"

#include <memory>
#include <array>

using namespace Study_DX;
using namespace DirectX;

void calculateNormal(std::vector<Vertex3D>& vertices, std::vector<DWORD> indices) {
	int vSize = vertices.size();
	int triangleNum = indices.size() / 3;

	//auto vertexNormal = std::make_shared<std::array<XMVECTOR, vSize>>();
	XMVECTOR *vertexNormal = new XMVECTOR[vSize];
	XMFLOAT3 zero = XMFLOAT3(0.0f, 0.0f, 0.0f);

	for (int i = 0; i < vSize; i++) {
		vertexNormal[i] = XMLoadFloat3(&zero);
	}

	for (int i = 0; i < triangleNum; i++) {
		int index0 = indices[i * 3 + 0];
		int index1 = indices[i * 3 + 1];
		int index2 = indices[i * 3 + 2];

		XMVECTOR pos0 = XMLoadFloat3(&vertices[index0].pos);
		XMVECTOR pos1 = XMLoadFloat3(&vertices[index1].pos);
		XMVECTOR pos2 = XMLoadFloat3(&vertices[index2].pos);

		XMVECTOR edge0 = XMVectorSubtract(pos1, pos0);
		XMVECTOR edge1 = XMVectorSubtract(pos2, pos0);
		XMVECTOR faceNormal = XMVector3Cross(edge0, edge1);

		vertexNormal[index0] = XMVectorAdd(vertexNormal[index0], faceNormal);
		vertexNormal[index1] = XMVectorAdd(vertexNormal[index1], faceNormal);
		vertexNormal[index2] = XMVectorAdd(vertexNormal[index2], faceNormal);
	}

	for (int i = 0; i < vSize; i++) {
		XMStoreFloat3(&vertices[i].normal, XMVector3Normalize(vertexNormal[i]));
	}
	delete[] vertexNormal;
}

Box::Box()
{
	calculateNormal(vertices, indices);
}

Box_Strange::Box_Strange() {
	//Study_DX::calculateNormal(vertices, vertexSize, indices, indexSize);
}

Study_DX::Sphere::Sphere(UINT _sliceCount, UINT _stackCount)
{
	float radius = 0.5f;

	Vertex3D topVertex(0.0f, +radius, 0.0f, 0.0f, 0.0f, 0.0f, +1.0f, 0.0f);
	Vertex3D bottomVertex(0.0f, -radius, 0.0f, 0.0f, 1.0f, 0.0f, -1.0f, 0.0f);


	vertices.push_back(topVertex);

	float phiStep = XM_PI / _stackCount;
	float thetaStep = 2.0f*XM_PI / _sliceCount;

	for (UINT i = 1; i <= _stackCount - 1; ++i)
	{
		float phi = i * phiStep;

		for (UINT j = 0; j <= _sliceCount; ++j)
		{
			float theta = j * thetaStep;

			Vertex3D v;

			v.pos.x = radius * sinf(phi)*cosf(theta);
			v.pos.y = radius * cosf(phi);
			v.pos.z = radius * sinf(phi)*sinf(theta);

			XMVECTOR p = XMLoadFloat3(&v.pos);
			XMStoreFloat3(&v.normal, XMVector3Normalize(p));

			v.texCoord.x = theta / XM_2PI;
			v.texCoord.y = phi / XM_PI;

			vertices.push_back(v);
		}
	}

	vertices.push_back(bottomVertex);

	for (UINT i = 1; i <= _sliceCount; ++i)
	{
		indices.push_back(0);
		indices.push_back(i + 1);
		indices.push_back(i);
	}

	UINT baseIndex = 1;
	UINT ringVertexCount = _sliceCount + 1;
	for (UINT i = 0; i < _stackCount - 2; ++i)
	{
		for (UINT j = 0; j < _sliceCount; ++j)
		{
			indices.push_back(baseIndex + i * ringVertexCount + j);
			indices.push_back(baseIndex + i * ringVertexCount + j + 1);
			indices.push_back(baseIndex + (i + 1)*ringVertexCount + j);

			indices.push_back(baseIndex + (i + 1)*ringVertexCount + j);
			indices.push_back(baseIndex + i * ringVertexCount + j + 1);
			indices.push_back(baseIndex + (i + 1)*ringVertexCount + j + 1);
		}
	}

	UINT southPoleIndex = (UINT)vertices.size() - 1;

	baseIndex = southPoleIndex - ringVertexCount;

	for (UINT i = 0; i < _sliceCount; ++i)
	{
		indices.push_back(southPoleIndex);
		indices.push_back(baseIndex + i);
		indices.push_back(baseIndex + i + 1);
	}
}

Study_DX::Cylinder::Cylinder(float bottomRadius , float topRadius, float height, UINT sliceCount, UINT stackCount) {
	float stackHeight = height / stackCount;

	// Amount to increment radius as we move up each stack level from bottom to top.
	float radiusStep = (topRadius - bottomRadius) / stackCount;

	UINT ringCount = stackCount + 1;

	// Compute vertices for each stack ring starting at the bottom and moving up.
	for (UINT i = 0; i < ringCount; ++i)
	{
		float y = -0.5f*height + i * stackHeight;
		float r = bottomRadius + i * radiusStep;

		// vertices of ring
		float dTheta = 2.0f*XM_PI / sliceCount;
		for (UINT j = 0; j <= sliceCount; ++j)
		{
			Vertex3D vertex;

			float c = cosf(j*dTheta);
			float s = sinf(j*dTheta);

			vertex.pos = XMFLOAT3(r*c, y, r*s);

			vertex.texCoord.x = (float)j / sliceCount;
			vertex.texCoord.y = 1.0f - (float)i / stackCount;

			XMFLOAT3 TangentU = XMFLOAT3(-s, 0.0f, c);

			float dr = bottomRadius - topRadius;
			XMFLOAT3 bitangent(dr*c, -height, dr*s);

			XMVECTOR T = XMLoadFloat3(&TangentU);
			XMVECTOR B = XMLoadFloat3(&bitangent);
			XMVECTOR N = XMVector3Normalize(XMVector3Cross(T, B));
			XMStoreFloat3(&vertex.normal, N);

			vertices.push_back(vertex);
		}
	}

	// Add one because we duplicate the first and last vertex per ring
	// since the texture coordinates are different.
	UINT ringVertexCount = sliceCount + 1;

	// Compute indices for each stack.
	for (UINT i = 0; i < stackCount; ++i)
	{
		for (UINT j = 0; j < sliceCount; ++j)
		{
			indices.push_back(i*ringVertexCount + j);
			indices.push_back((i + 1)*ringVertexCount + j);
			indices.push_back((i + 1)*ringVertexCount + j + 1);
			
			indices.push_back(i*ringVertexCount + j);
			indices.push_back((i + 1)*ringVertexCount + j + 1);
			indices.push_back(i*ringVertexCount + j + 1);
		}
	}

	BuildCylinderTopCap(bottomRadius, topRadius, height, sliceCount, stackCount);
	BuildCylinderBottomCap(bottomRadius, topRadius, height, sliceCount, stackCount);
}

void Study_DX::Cylinder::BuildCylinderTopCap(float bottomRadius, float topRadius, float height, UINT sliceCount, UINT stackCount)
{
	UINT baseIndex = (UINT)vertices.size();

	float y = 0.5f*height;
	float dTheta = 2.0f*XM_PI / sliceCount;

	for (UINT i = 0; i <= sliceCount; ++i)
	{
		float x = topRadius * cosf(i*dTheta);
		float z = topRadius * sinf(i*dTheta);

		float u = x / height + 0.5f;
		float v = z / height + 0.5f;

		vertices.push_back(Vertex3D(x, y, z, u, v, 0.0f, 1.0f, 0.0f));
	}

	vertices.push_back(Vertex3D(0.0f, y, 0.0f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f));

	UINT centerIndex = (UINT)vertices.size() - 1;

	for (UINT i = 0; i < sliceCount; ++i)
	{
		indices.push_back(centerIndex);
		indices.push_back(baseIndex + i + 1);
		indices.push_back(baseIndex + i);
	}
}

void Study_DX::Cylinder::BuildCylinderBottomCap(float bottomRadius, float topRadius, float height, UINT sliceCount, UINT stackCount)
{
	UINT baseIndex = (UINT)vertices.size();
	float y = -0.5f*height;

	float dTheta = 2.0f*XM_PI / sliceCount;
	for (UINT i = 0; i <= sliceCount; ++i)
	{
		float x = bottomRadius * cosf(i*dTheta);
		float z = bottomRadius * sinf(i*dTheta);

		float u = x / height + 0.5f;
		float v = z / height + 0.5f;

		vertices.push_back(Vertex3D(x, y, z, u, v, 0.0f, -1.0f, 0.0f));
	}

	vertices.push_back(Vertex3D(0.0f, y, 0.0f, 0.5f, 0.5f, 0.0f, -1.0f, 0.0f));

	UINT centerIndex = (UINT)vertices.size() - 1;

	for (UINT i = 0; i < sliceCount; ++i)
	{
		indices.push_back(centerIndex);
		indices.push_back(baseIndex + i);
		indices.push_back(baseIndex + i + 1);
	}
}






