#pragma once
#include "Vertex.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "ConstantBuffer.h"
#include "Texture.h"

class Mesh {
public:
	Mesh(
		const std::vector<Vertex3D> & vertices, 
		const std::vector<DWORD> & indices, 
		const std::vector<Texture> & textures, 
		const DirectX::XMMATRIX & transformMatrix);
	Mesh(const Mesh & mesh);

	const DirectX::XMMATRIX & GetTransformMatrix() const	{ return transformMatrix; }
	const VertexBuffer<Vertex3D> & GetVertexBuffer() const	{ return vertexbuffer; }
	const IndexBuffer & GetIndexBuffer() const				{ return indexbuffer; }
	const std::vector<Texture> & GetTextures() const		{ return textures; }

private:
	VertexBuffer<Vertex3D> vertexbuffer;
	IndexBuffer indexbuffer;
	std::vector<Texture> textures;
	DirectX::XMMATRIX transformMatrix;
};

class SkinnedMesh {
public:
	SkinnedMesh(
		const std::vector<SkinnedVertex> & vertices,
		const std::vector<DWORD> & indices,
		const std::vector<Texture> & textures,
		const DirectX::XMMATRIX & transformMatrix);
	SkinnedMesh(const SkinnedMesh & mesh);

	const DirectX::XMMATRIX & GetTransformMatrix() const { return transformMatrix; }
	const VertexBuffer<SkinnedVertex> & GetVertexBuffer() const { return vertexbuffer; }
	const IndexBuffer & GetIndexBuffer() const { return indexbuffer; }
	const std::vector<Texture> & GetTextures() const { return textures; }

private:
	VertexBuffer<SkinnedVertex> vertexbuffer;
	IndexBuffer indexbuffer;
	std::vector<Texture> textures;
	DirectX::XMMATRIX transformMatrix;
};