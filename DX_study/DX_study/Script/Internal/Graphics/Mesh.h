#pragma once
#include "Vertex.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "ConstantBuffer.h"
#include "Texture.h"
#include "../Core/Object.h"
#include <DirectXCollision.h>

struct ID3D11Buffer;

class MeshBase : public Object {
	MANAGED_OBJECT(MeshBase)
public:
	MeshBase(
		const std::vector<DWORD> & indices,
		const DirectX::XMMATRIX & transformMatrix,
		const std::string & name = "Mesh");
	MeshBase(const MeshBase& mesh);

	virtual ID3D11Buffer* const*						GetVertexBufferAddr() const = 0;
	virtual const UINT *								GetVertexBufferStridePtr() const = 0;
	const DirectX::XMMATRIX &							GetTransformMatrix() const { return transformMatrix; }
	const IndexBuffer &									GetIndexBuffer() const { return indexbuffer; }
	const DirectX::BoundingBox &						GetLocalAABB() const { return m_Aabb; }

protected:
	IndexBuffer indexbuffer;
	DirectX::XMMATRIX transformMatrix;
	DirectX::BoundingBox m_Aabb;
};

class Mesh : public MeshBase {
public:
	Mesh(
		const std::vector<Vertex3D> & vertices, 
		const std::vector<DWORD> & indices, 
		const DirectX::XMMATRIX & transformMatrix,
		const std::string & name = "Mesh");
	Mesh(const Mesh & mesh);

	ID3D11Buffer* const*	GetVertexBufferAddr() const override { return vertexbuffer.GetAddressOf(); }
	const UINT *				GetVertexBufferStridePtr() const override { return vertexbuffer.StridePtr(); }
	const VertexBuffer<Vertex3D> & GetVertexBuffer() const	{ return vertexbuffer; }

private:
	VertexBuffer<Vertex3D> vertexbuffer;
};

class SkinnedMesh : public MeshBase {
public:
	SkinnedMesh(
		const std::vector<SkinnedVertex> & vertices,
		const std::vector<DWORD> & indices,
		const DirectX::XMMATRIX & transformMatrix,
		const std::string & name = "Mesh");
	SkinnedMesh(const SkinnedMesh & mesh);

	ID3D11Buffer* const*	GetVertexBufferAddr() const override { return vertexbuffer.GetAddressOf(); }
	const UINT *				GetVertexBufferStridePtr() const override { return vertexbuffer.StridePtr(); }
	const VertexBuffer<SkinnedVertex> & GetVertexBuffer() const { return vertexbuffer; }

private:
	VertexBuffer<SkinnedVertex> vertexbuffer;
};