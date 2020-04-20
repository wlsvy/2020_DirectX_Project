#pragma once
#include "Vertex.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "ConstantBuffer.h"
#include "Texture.h"
#include "../Core/Object.h"

struct AABB {
	AABB() {}
	AABB(
		float max_x, float max_y, float max_z,
		float min_x, float min_y, float min_z) :
		Min(min_x, min_y, min_z),
		Max(max_x, max_y, max_z) {}

	DirectX::XMFLOAT3 Min = DirectX::XMFLOAT3(-9999.0f, -9999.0f, -9999.0f);
	DirectX::XMFLOAT3 Max = DirectX::XMFLOAT3(9999.0f, 9999.0f, 9999.0f);
};

class MeshBase : public Object {
public:
	MeshBase(
		const std::vector<DWORD> & indices,
		const std::vector<Texture> & textures,
		const DirectX::XMMATRIX & transformMatrix,
		const std::string & name = "Mesh");
	MeshBase(const MeshBase& mesh);

	const DirectX::XMMATRIX & GetTransformMatrix() const { return transformMatrix; }
	const IndexBuffer & GetIndexBuffer() const { return indexbuffer; }
	const std::vector<Texture> & GetTextures() const { return textures; }

protected:
	IndexBuffer indexbuffer;
	std::vector<Texture> textures;
	DirectX::XMMATRIX transformMatrix;
	AABB m_AABB;
};

class Mesh : public MeshBase {
public:
	Mesh(
		const std::vector<Vertex3D> & vertices, 
		const std::vector<DWORD> & indices, 
		const std::vector<Texture> & textures, 
		const DirectX::XMMATRIX & transformMatrix,
		const std::string & name = "Mesh");
	Mesh(const Mesh & mesh);

	const VertexBuffer<Vertex3D> & GetVertexBuffer() const	{ return vertexbuffer; }

private:
	VertexBuffer<Vertex3D> vertexbuffer;
};

class SkinnedMesh : public MeshBase {
public:
	SkinnedMesh(
		const std::vector<SkinnedVertex> & vertices,
		const std::vector<DWORD> & indices,
		const std::vector<Texture> & textures,
		const DirectX::XMMATRIX & transformMatrix,
		const std::string & name = "Mesh");
	SkinnedMesh(const SkinnedMesh & mesh);

	const VertexBuffer<SkinnedVertex> & GetVertexBuffer() const { return vertexbuffer; }

private:
	VertexBuffer<SkinnedVertex> vertexbuffer;
};