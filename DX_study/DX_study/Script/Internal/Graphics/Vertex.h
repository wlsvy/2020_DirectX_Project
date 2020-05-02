#pragma once
#include <DirectXMath.h>

struct Vertex2D {
	Vertex2D() {}
	Vertex2D(float x, float y, float z, float u, float v)
		: pos(x, y, z), texCoord(u, v) {}
	DirectX::XMFLOAT3 pos = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);
	DirectX::XMFLOAT2 texCoord = DirectX::XMFLOAT2(0.0f, 0.0f);
};

struct Vertex3D : Vertex2D {
	Vertex3D() {}
	Vertex3D(
		float x, float y, float z,
		float u, float v,
		float nx, float ny, float nz)
		:
		Vertex2D(x, y, z, u, v),
		normal(nx, ny, nz) {}
	DirectX::XMFLOAT3 normal = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);
	DirectX::XMFLOAT4 tangent = DirectX::XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
};

struct SkinnedVertex : Vertex3D {
	SkinnedVertex() {}
	SkinnedVertex(
		float x, float y, float z,
		float u, float v,
		float nx, float ny, float nz,
		int boneID0, int boneID1, int boneID2, int boneID3,
		float weight0, float weight1, float weight2, float weight3) :
		Vertex3D(x, y, z, u, v, nx, ny, nz),
		boneIDs(boneID0, boneID1, boneID2, boneID3),
		boneWeights(weight0, weight1, weight2, weight3) {}

	DirectX::XMINT4 boneIDs = DirectX::XMINT4(-1, -1, -1, -1);
	DirectX::XMFLOAT4 boneWeights = DirectX::XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f);
};