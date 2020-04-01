#pragma once
#include "Vertex.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "ConstantBuffer.h"
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include "Texture.h"

class Mesh {
public:
	Mesh(
		std::vector<Vertex3D> & vertices, 
		std::vector<DWORD> & indices, 
		std::vector<Texture> & textures, 
		const DirectX::XMMATRIX & transformMatrix);
	Mesh(const Mesh & mesh);

	void Draw();

	const DirectX::XMMATRIX & GetTransformMatrix() const;
	const VertexBuffer<Vertex3D> & GetVertexBuffer() const;
	const IndexBuffer & GetIndexBuffer() const;
	const std::vector<Texture> & GetTextures() const;

private:
	VertexBuffer<Vertex3D> vertexbuffer;
	IndexBuffer indexbuffer;
	std::vector<Texture> textures;
	DirectX::XMMATRIX transformMatrix;
};