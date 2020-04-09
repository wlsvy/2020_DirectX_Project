#include "Mesh.h"

Mesh::Mesh(
	const std::vector<Vertex3D>& vertices, 
	const std::vector<DWORD>& indices, 
	const std::vector<Texture> & textures, 
	const DirectX::XMMATRIX & transformMatrix)
	:
	textures(textures),
	transformMatrix(transformMatrix)
{
	try {
		HRESULT hr = this->vertexbuffer.Initialize(vertices.data(), vertices.size());
		ThrowIfFailed(hr, "Failed to initialize vertex buffer for mesh.");

		hr = this->indexbuffer.Initialize(indices.data(), indices.size());
		ThrowIfFailed(hr, "Failed to initialize index buffer for mesh.");
	}
	catch (COMException e) {
		ErrorLogger::Log(e);
	}
}

Mesh::Mesh(const Mesh & mesh) :
	indexbuffer(mesh.indexbuffer),
	vertexbuffer(mesh.vertexbuffer),
	textures(mesh.textures),
	transformMatrix(mesh.transformMatrix)
{
}

SkinnedMesh::SkinnedMesh(
	const std::vector<Vertex3D_BoneWeight>& vertices, 
	const std::vector<DWORD>& indices, 
	const std::vector<Texture>& textures, 
	const DirectX::XMMATRIX & transformMatrix)
	:
	textures(textures),
	transformMatrix(transformMatrix)
{
	try {
		HRESULT hr = this->vertexbuffer.Initialize(vertices.data(), vertices.size());
		ThrowIfFailed(hr, "Failed to initialize vertex buffer for SkinnedMesh.");

		hr = this->indexbuffer.Initialize(indices.data(), indices.size());
		ThrowIfFailed(hr, "Failed to initialize index buffer for SkinnedMesh.");
	}
	catch (COMException e) {
		ErrorLogger::Log(e);
	}
}

SkinnedMesh::SkinnedMesh(const SkinnedMesh & mesh) :
	indexbuffer(mesh.indexbuffer),
	vertexbuffer(mesh.vertexbuffer),
	textures(mesh.textures),
	transformMatrix(mesh.transformMatrix)
{
}
