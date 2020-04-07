#include "Mesh.h"

Mesh::Mesh(
	std::vector<Vertex3D>& vertices, 
	std::vector<DWORD>& indices, 
	std::vector<Texture> & textures, 
	const DirectX::XMMATRIX & transformMatrix)
{
	this->textures = textures;
	this->transformMatrix = transformMatrix;

	try {
		HRESULT hr = this->vertexbuffer.Initialize(vertices.data(), vertices.size());
		ThrowIfFailed(hr, "Failed to initialize vertex buffer for mesh.");

		hr = this->indexbuffer.Initialize(indices.data(), indices.size());
		ThrowIfFailed(hr, "Failed to initialize index buffer for mesh.");
	}
	catch (COMException e) {
	}
	
}

Mesh::Mesh(const Mesh & mesh)
{
	this->indexbuffer = mesh.indexbuffer;
	this->vertexbuffer = mesh.vertexbuffer;
	this->textures = mesh.textures;
	this->transformMatrix = mesh.transformMatrix;
}