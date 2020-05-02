#pragma once
#include "Vertex.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "../Core/Object.h"
#include "../../Util/Math.h"
#include "../Core/InternalHelper.h"
#include "../Engine/AdapterReader.h"
#include "imGui/imgui.h"

class Mesh : public Object {
	MANAGED_OBJECT(Mesh)
public:
	Mesh(
		const std::vector<DWORD> & indices,
		const DirectX::XMMATRIX & worldMatrix,
		const std::string & name = "Mesh");

	virtual ID3D11Buffer* const*	GetVertexBufferAddr() const = 0;
	virtual const UINT *			GetVertexBufferStridePtr() const = 0;
	const DirectX::XMMATRIX &		GetWorldMatrix() const { return m_WorldMatrix; }
	const IndexBuffer &				GetIndexBuffer() const { return m_IndexBuffer; }
	const DirectX::BoundingBox &	GetLocalAABB() const { return m_Aabb; }

protected:
	IndexBuffer m_IndexBuffer;
	DirectX::XMMATRIX m_WorldMatrix;
	DirectX::BoundingBox m_Aabb;
};

template<typename VertexTy>
class MeshReal : public Mesh {
public:
	MeshReal(
		const std::vector<VertexTy> & vertices,
		const std::vector<DWORD> & indices,
		const DirectX::XMMATRIX & worldMatrix,
		const std::string & name = "Mesh");
	void OnGui() override;

	ID3D11Buffer* const*			GetVertexBufferAddr() const override { return vertexbuffer.GetAddressOf(); }
	const UINT *					GetVertexBufferStridePtr() const override { return vertexbuffer.StridePtr(); }

private:
	VertexBuffer<VertexTy> vertexbuffer;
};

template<typename VertexTy>
inline MeshReal<VertexTy>::MeshReal(
	const std::vector<VertexTy>& vertices, 
	const std::vector<DWORD>& indices, 
	const DirectX::XMMATRIX & worldMatrix,
	const std::string & name) :
	Mesh(indices, worldMatrix, name)
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
			vertexbuffer.Initialize(vertices.data(), (UINT)vertices.size()),
			"Failed to initialize vertex buffer for SkinnedMesh.");
	}
	catch (CustomException e) {
		StringHelper::ErrorLog(e);
	}
}

template<typename VertexTy>
inline void MeshReal<VertexTy>::OnGui()
{
	ImGui::Text(("Mesh : " + Name).c_str());
}
