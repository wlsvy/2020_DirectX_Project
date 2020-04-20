#include "Mesh.h"

Mesh::Mesh(
	const std::vector<Vertex3D>& vertices, 
	const std::vector<DWORD>& indices, 
	const std::vector<Texture> & textures, 
	const DirectX::XMMATRIX & transformMatrix,
	const std::string & name)
	:
	MeshBase(indices, textures, transformMatrix, name)
{
	for (auto vertex : vertices) {
		m_AABB.Max.x = std::max(m_AABB.Max.x, vertex.pos.x);
		m_AABB.Max.y = std::max(m_AABB.Max.y, vertex.pos.y);
		m_AABB.Max.z = std::max(m_AABB.Max.z, vertex.pos.z);
		m_AABB.Min.x = std::min(m_AABB.Min.x, vertex.pos.x);
		m_AABB.Min.y = std::min(m_AABB.Min.y, vertex.pos.y);
		m_AABB.Min.z = std::min(m_AABB.Min.z, vertex.pos.z);
	}

	try {
		ThrowIfFailed(
			vertexbuffer.Initialize(vertices.data(), vertices.size()),
			"Failed to initialize vertex buffer for SkinnedMesh.");
	}
	catch (CustomException e) {
		ErrorLogger::Log(e);
	}
}

Mesh::Mesh(const Mesh & mesh) : 
	MeshBase(mesh),
	vertexbuffer(mesh.vertexbuffer)
{
}

SkinnedMesh::SkinnedMesh(
	const std::vector<SkinnedVertex>& vertices, 
	const std::vector<DWORD>& indices, 
	const std::vector<Texture>& textures, 
	const DirectX::XMMATRIX & transformMatrix,
	const std::string & name)
	:
	MeshBase(indices, textures, transformMatrix, name)
{
	for (auto vertex : vertices) {
		m_AABB.Max.x = std::max(m_AABB.Max.x, vertex.pos.x);
		m_AABB.Max.y = std::max(m_AABB.Max.y, vertex.pos.y);
		m_AABB.Max.z = std::max(m_AABB.Max.z, vertex.pos.z);
		m_AABB.Min.x = std::min(m_AABB.Min.x, vertex.pos.x);
		m_AABB.Min.y = std::min(m_AABB.Min.y, vertex.pos.y);
		m_AABB.Min.z = std::min(m_AABB.Min.z, vertex.pos.z);
	}

	try {
		ThrowIfFailed(
			vertexbuffer.Initialize(vertices.data(), vertices.size()), 
			"Failed to initialize vertex buffer for SkinnedMesh.");
	}
	catch (CustomException e) {
		ErrorLogger::Log(e);
	}
}

SkinnedMesh::SkinnedMesh(const SkinnedMesh & mesh) :
	MeshBase(mesh),
	vertexbuffer(mesh.vertexbuffer)
{
}

MeshBase::MeshBase(
	const std::vector<DWORD>& indices,
	const std::vector<Texture>& textures,
	const DirectX::XMMATRIX & transformMatrix,
	const std::string & name)
	:
	Object(name),
	textures(textures),
	transformMatrix(transformMatrix)
{
	try {
		ThrowIfFailed(
			this->indexbuffer.Initialize(indices.data(), indices.size())
			, "Failed to initialize index buffer for mesh.");
	}
	catch (CustomException e) {
		ErrorLogger::Log(e);
	}
}

MeshBase::MeshBase(const MeshBase& mesh) :
	Object(mesh),
	indexbuffer(mesh.indexbuffer),
	textures(mesh.textures),
	transformMatrix(mesh.transformMatrix),
	m_AABB(mesh.m_AABB)
{
}
