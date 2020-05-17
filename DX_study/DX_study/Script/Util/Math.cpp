#include "Math.h"

#include <DirectXMesh/DirectXMeshP.h>
#include <DirectXMesh/DirectXMeshTangentFrame.cpp>
#include <DirectXMesh/DirectXMeshNormals.cpp>
#include "../Internal/Graphics/Mesh.h"
#include "../Component/Transform.h"
#include "../Component/RenderInfo.h"



using DirectX::operator+;
using DirectX::operator-;
using DirectX::operator/=;

float Math::GetDistance(const Transform & src, const DirectX::XMFLOAT3 & target)
{
	DirectX::XMVECTOR vec = DirectX::XMLoadFloat3(&target);
	return GetDistance(src, vec);
}

float Math::GetDistance(const Transform & src, const DirectX::XMVECTOR & target)
{
	auto diff = target -src.positionVec;
	return DirectX::XMVector3Length(diff).m128_f32[0];
}

float Math::GetDistance(const Transform & src, const Transform & target)
{
	auto diff = target.positionVec - src.positionVec;
	return DirectX::XMVector3Length(diff).m128_f32[0];
}

float Math::GetDistance(const DirectX::XMVECTOR & src, const DirectX::XMVECTOR & target)
{
	return DirectX::XMVector3Length(target - src).m128_f32[0];
}

DirectX::BoundingBox Math::GetGlobalBoundingBox(const DirectX::BoundingBox & box, const Transform & tf)
{
	auto centerVec = DirectX::XMLoadFloat3(&box.Center);
	auto newCenterVec = DirectX::XMVector3Transform(centerVec, tf.GetWorldMatrix());
	DirectX::XMFLOAT3 newCenter;
	DirectX::XMStoreFloat3(&newCenter, newCenterVec);

	auto globalScale = tf.GetLossyScale();

	return DirectX::BoundingBox(
		newCenter,
		DirectX::XMFLOAT3(
			box.Extents.x * globalScale.m128_f32[0],
			box.Extents.y * globalScale.m128_f32[1],
			box.Extents.z * globalScale.m128_f32[2]
		)
	);
}

template<typename VertexTy>
void Math::ComputeVertexNormal(std::vector<VertexTy> & vertices, std::vector<DWORD> & indices) {
	size_t vertSize = vertices.size();
	size_t faceSize = indices.size() / 3;

	std::unique_ptr<unsigned int[]> indiciesI32 = std::make_unique<unsigned int[]>(indices.size());
	for (size_t i = 0; i < indices.size(); i++) {
		indiciesI32[i] = (unsigned int)indices[i];
	}
	std::unique_ptr<DirectX::XMFLOAT3[]> pos = std::make_unique<DirectX::XMFLOAT3[]>(vertSize);
	for (size_t i = 0; i < vertSize; ++i) {
		pos[i] = vertices[i].pos;
	}

	std::unique_ptr<DirectX::XMFLOAT3[]> normals = std::make_unique<DirectX::XMFLOAT3[]>(vertSize);

	ThrowIfFailed(
		DirectX::ComputeNormals(
			indiciesI32.get(), faceSize,
			pos.get(), vertSize,
			DirectX::CNORM_DEFAULT, normals.get()),
		"Failed To Compute Normals");

	for (size_t i = 0; i < vertSize; ++i) {
		vertices[i].normal = normals[i];
	}
}

template<typename VertexTy>
void Math::ComputeVertexTangent(std::vector<VertexTy> & vertices, std::vector<DWORD> & indices) {
	size_t vertSize = vertices.size();
	size_t faceSize = indices.size() / 3;

	std::unique_ptr<unsigned int[]> indiciesI32 = std::make_unique<unsigned int[]>(indices.size());
	for (size_t i = 0; i < indices.size(); i++) {
		indiciesI32[i] = (unsigned int)indices[i];
	}
	std::unique_ptr<DirectX::XMFLOAT3[]> pos = std::make_unique<DirectX::XMFLOAT3[]>(vertSize);
	for (size_t i = 0; i < vertSize; ++i) {
		pos[i] = vertices[i].pos;
	}
	std::unique_ptr<DirectX::XMFLOAT3[]> normals = std::make_unique<DirectX::XMFLOAT3[]>(vertSize);
	for (size_t i = 0; i < vertSize; ++i) {
		normals[i] = vertices[i].normal;
	}
	std::unique_ptr<DirectX::XMFLOAT2[]> texcoords = std::make_unique< DirectX::XMFLOAT2[]>(vertSize);
	for (size_t i = 0; i < vertSize; ++i) {
		texcoords[i] = vertices[i].texCoord;
	}

	std::unique_ptr<DirectX::XMFLOAT4[]> tangents = std::make_unique<DirectX::XMFLOAT4[]>(vertSize);
	std::unique_ptr<DirectX::XMFLOAT3[]> bitangents = std::make_unique<DirectX::XMFLOAT3[]>(vertSize);

	ThrowIfFailed(
		DirectX::ComputeTangentFrame(
			indiciesI32.get(), faceSize,
			pos.get(),
			normals.get(),
			texcoords.get(), vertSize,
			tangents.get(),
			bitangents.get()),
		"Failed To Compute Tangent");

	for (size_t i = 0; i < vertSize; ++i) {
		vertices[i].tangent.x = tangents[i].x;
		vertices[i].tangent.y = tangents[i].y;
		vertices[i].tangent.z = tangents[i].z;
	}
}

void TemplateTypeDeclare() {
	std::vector<DWORD> indices;

	{
		std::vector<SkinnedVertex> vertices;
		Math::ComputeVertexNormal(vertices, indices);
		Math::ComputeVertexTangent<SkinnedVertex>(vertices, indices);
	}
	{
		std::vector<Vertex3D> vertices;
		Math::ComputeVertexNormal(vertices, indices);
		Math::ComputeVertexTangent<Vertex3D>(vertices, indices);
	}
}

