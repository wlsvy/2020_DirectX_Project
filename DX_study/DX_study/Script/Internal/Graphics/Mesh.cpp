#include "Mesh.h"

#include "../../Util/Math.h"
#include "../Core/InternalHelper.h"
#include "../Engine/AdapterReader.h"

Mesh::Mesh(
	const std::vector<Vertex3D>& vertices, 
	const std::vector<DWORD>& indices, 
	const std::vector<std::shared_ptr<Texture>> & textures, 
	const DirectX::XMMATRIX & transformMatrix,
	const std::string & name)
	:
	MeshBase(indices, textures, transformMatrix, name)
{
	auto Max = DirectX::XMFLOAT3(Math::POSITION_MIN, Math::POSITION_MIN, Math::POSITION_MIN);
	auto Min = DirectX::XMFLOAT3(Math::POSITION_MAX, Math::POSITION_MAX, Math::POSITION_MAX);

	for (auto vertex : vertices) {
		Max.x = std::max(Max.x, vertex.pos.x);
		Max.y = std::max(Max.y, vertex.pos.y);
		Max.z = std::max(Max.z, vertex.pos.z);
		Min.x = std::min(Min.x, vertex.pos.x);
		Min.y = std::min(Min.y, vertex.pos.y);
		Min.z = std::min(Min.z, vertex.pos.z);
	}
	m_Aabb.Center.x = (Max.x + Min.x) / 2;
	m_Aabb.Center.y = (Max.y + Min.y) / 2;
	m_Aabb.Center.z = (Max.z + Min.z) / 2;
	m_Aabb.Extents.x = (Max.x - Min.x) / 2;
	m_Aabb.Extents.y = (Max.y - Min.y) / 2;
	m_Aabb.Extents.z = (Max.z - Min.z) / 2;

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
	const std::vector<std::shared_ptr<Texture>>& textures, 
	const DirectX::XMMATRIX & transformMatrix,
	const std::string & name)
	:
	MeshBase(indices, textures, transformMatrix, name)
{
	auto Max = DirectX::XMFLOAT3(Math::POSITION_MIN, Math::POSITION_MIN, Math::POSITION_MIN);
	auto Min = DirectX::XMFLOAT3(Math::POSITION_MAX, Math::POSITION_MAX, Math::POSITION_MAX);

	for (auto vertex : vertices) {
		Max.x = std::max(Max.x, vertex.pos.x);
		Max.y = std::max(Max.y, vertex.pos.y);
		Max.z = std::max(Max.z, vertex.pos.z);
		Min.x = std::min(Min.x, vertex.pos.x);
		Min.y = std::min(Min.y, vertex.pos.y);
		Min.z = std::min(Min.z, vertex.pos.z);
	}
	m_Aabb.Center.x = (Max.x + Min.x) / 2;
	m_Aabb.Center.y = (Max.y + Min.y) / 2;
	m_Aabb.Center.z = (Max.z + Min.z) / 2;
	m_Aabb.Extents.x = (Max.x - Min.x) / 2;
	m_Aabb.Extents.y = (Max.y - Min.y) / 2;
	m_Aabb.Extents.z = (Max.z - Min.z) / 2;

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
	const std::vector<std::shared_ptr<Texture>>& textures,
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
	m_Aabb(mesh.m_Aabb)
{
}