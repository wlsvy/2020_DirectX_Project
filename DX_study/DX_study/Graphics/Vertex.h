#pragma once
#include <DirectXMath.h>

struct Vertex2D {
	Vertex2D() {}
	Vertex2D(float x, float y, float z, float u, float v)
		: pos(x, y, z), texCoord(u, v) {}
	DirectX::XMFLOAT3 pos;
	DirectX::XMFLOAT2 texCoord;
};

struct Vertex3D {
	Vertex3D() {}
	//Vertex(float x, float y, float z, float r, float g, float b) : pos(x, y, z), color(r,g,b) {}
	Vertex3D(float x, float y, float z, float u, float v, float nx, float ny, float nz)
		: pos(x, y, z), texCoord(u, v), normal(nx, ny, nz) {}
	DirectX::XMFLOAT3 pos;
	//DirectX::XMFLOAT3 color;
	DirectX::XMFLOAT2 texCoord;
	DirectX::XMFLOAT3 normal;
};

struct Vertex3D_BoneWeight {
	Vertex3D_BoneWeight() {}
	Vertex3D_BoneWeight(float x, float y, float z, float u, float v, float nx, float ny, float nz,
		int _id1, int _id2, int _id3, int _id4, float _weight1, float _weight2, float _weight3, float _weight4)
		: pos(x, y, z), texCoord(u, v), normal(nx, ny, nz),
		boneIDs(_id1, _id2, _id3, _id4), 
		boneWeights(_weight1, _weight2, _weight3, _weight4) {}

	Vertex3D_BoneWeight(DirectX::XMFLOAT3 & _pos, DirectX::XMFLOAT2 & _texCoord, DirectX::XMFLOAT3 & _normal)
		: pos(_pos), texCoord(_texCoord), normal(_normal) {}

	Vertex3D_BoneWeight(DirectX::XMFLOAT3 & _pos, DirectX::XMFLOAT2 & _texCoord, DirectX::XMFLOAT3 & _normal, 
		DirectX::XMINT4 & _boneIDs,
		DirectX::XMFLOAT4 & _boneWeights)
		: pos(_pos), texCoord(_texCoord), normal(_normal), 
		boneIDs(_boneIDs), boneWeights(_boneWeights) {}

	DirectX::XMFLOAT3 pos;
	DirectX::XMFLOAT2 texCoord;
	DirectX::XMFLOAT3 normal;

	DirectX::XMINT4 boneIDs = DirectX::XMINT4(-1, -1, -1, -1);
	DirectX::XMFLOAT4 boneWeights = DirectX::XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f);
};