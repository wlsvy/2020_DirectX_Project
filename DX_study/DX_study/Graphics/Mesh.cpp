#include "Mesh.h"
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>

#include "../Engine/ModuleResource.h"

Mesh::Mesh(std::vector<Vertex3D>& vertices, std::vector<DWORD>& indices, std::vector<Texture*> & textures, const DirectX::XMMATRIX & transformMatrix, MyCustom::DRAW_FLAG _drawflag)
{
	this->textures = textures;
	this->transformMatrix = transformMatrix;
	hasBone = false;

	DRAW_MESH = _drawflag == 0 ? &Mesh::Draw_Default : &Mesh::Draw_NoTexture;

	try {
		HRESULT hr = this->vertexbuffer.Initialize(&Module::GetDevice(), vertices.data(), vertices.size());
		COM_ERROR_IF_FAILED(hr, "Failed to initialize vertex buffer for mesh.");

		hr = this->indexbuffer.Initialize(&Module::GetDevice(), indices.data(), indices.size());
		COM_ERROR_IF_FAILED(hr, "Failed to initialize index buffer for mesh.");
	}
	catch (COMException e) {
		ErrorLogger::Log(e);
		return ;
	}
	
}

Mesh::Mesh(Vertex3D * _vertices, const UINT _vertexSize, DWORD * _indices, const UINT _indexSize, std::vector<Texture*>& textures, const DirectX::XMMATRIX & transformMatrix, MyCustom::DRAW_FLAG _drawflag)
{
	this->textures = textures;
	this->transformMatrix = transformMatrix;
	hasBone = false;
	DRAW_MESH = _drawflag == 0 ? &Mesh::Draw_Default : &Mesh::Draw_NoTexture;

	try {
		HRESULT hr = this->vertexbuffer.Initialize(&Module::GetDevice(), _vertices, _vertexSize);
		COM_ERROR_IF_FAILED(hr, "Failed to initialize vertex buffer for mesh.");

		hr = this->indexbuffer.Initialize(&Module::GetDevice(), _indices, _indexSize);
		COM_ERROR_IF_FAILED(hr, "Failed to initialize index buffer for mesh.");
	}
	catch (COMException e) {
		ErrorLogger::Log(e);
		return;
	}
}

Mesh::Mesh(std::vector<Vertex3D_BoneWeight>& _vert_bones, std::vector<DWORD>& indices, std::vector<Texture*>& textures, const DirectX::XMMATRIX & transformMatrix)
{
	this->textures = textures;
	this->transformMatrix = transformMatrix;
	hasBone = true;
	DRAW_MESH = &Mesh::Draw_SkinnedMesh;

	try {
		HRESULT hr = this->vertex_BoneWeight_buffer.Initialize(&Module::GetDevice(), _vert_bones.data(), _vert_bones.size());
		COM_ERROR_IF_FAILED(hr, "Failed to initialize vertex buffer for mesh.");

		hr = this->indexbuffer.Initialize(&Module::GetDevice(), indices.data(), indices.size());
		COM_ERROR_IF_FAILED(hr, "Failed to initialize index buffer for mesh.");
	}
	catch (COMException e) {
		ErrorLogger::Log(e);
		return;
	}
}

Mesh::Mesh(Vertex3D & _vertex, std::vector<Texture*>& textures, const DirectX::XMMATRIX & transformMatrix)
{
	this->textures = textures;
	this->transformMatrix = transformMatrix;
	hasBone = false;
	DRAW_MESH = &Mesh::Draw_Billboard;

	try {
		HRESULT hr = this->vertexbuffer.Initialize(&Module::GetDevice(), &_vertex, 1);
		COM_ERROR_IF_FAILED(hr, "Failed to initialize vertex buffer for mesh.");
	}
	catch (COMException e) {
		ErrorLogger::Log(e);
		return;
	}
}

Mesh::Mesh(const Mesh & mesh)
{
	this->indexbuffer = mesh.indexbuffer;
	this->vertexbuffer = mesh.vertexbuffer;
	this->vertex_BoneWeight_buffer = mesh.vertex_BoneWeight_buffer;
	this->textures = mesh.textures;
	this->transformMatrix = mesh.transformMatrix;
	this->hasBone = mesh.hasBone;
	this->DRAW_MESH = mesh.DRAW_MESH;
}

void Mesh::Draw()
{
	(this->*DRAW_MESH)();
}

void Mesh::Draw_Default()
{
	UINT offset = 0;

	for (int i = 0; i < textures.size(); i++) {//���� ���߿� ����ȭ
		if (textures[i]->GetType() == aiTextureType::aiTextureType_DIFFUSE) {
			Module::GetDeviceContext().PSSetShaderResources(i, 1, textures[i]->GetTextureResourceViewAddress()); //�ȼ� ���̴��� �ؽ��� ����
			break;
		}
	}

	Module::GetDeviceContext().IASetVertexBuffers(0, 1, this->vertexbuffer.GetAddressOf(), this->vertexbuffer.StridePtr(), &offset);
	
	//Module::GetDeviceContext().IASetVertexBuffers(0, 1, this->vertex_BoneWeight_buffer.GetAddressOf(), this->vertex_BoneWeight_buffer.StridePtr(), &offset);
	
	/* ������ ������ �������� ������ �������������� �����ϱ� ���� ���۸� ��ġ�� �� �Է� ���Կ� ����.
	virtual void STDMETHODCALLTYPE IASetVertexBuffers( 
	_In_range_(0, D3D11_IA_VERTEX_INPUT_RESOURCE_SLOT_COUNT - 1)  UINT StartSlot,	//���� ���۵��� ���̱� ������ �Է� ������ ����
		_In_range_(0, D3D11_IA_VERTEX_INPUT_RESOURCE_SLOT_COUNT - StartSlot)  UINT NumBuffers,	//�� ȣ�⿡�� �Է� ���Ե鿡 ���� ���۰���
		_In_reads_opt_(NumBuffers)  ID3D11Buffer *const *ppVertexBuffers,	//���� ���� ���۵��� ���� �迭�� ù ���Ҹ� ����Ű�� ������
		_In_reads_opt_(NumBuffers)  const UINT *pStrides,	//�������� �迭�� ù ���Ҹ� ����Ű�� ������
		_In_reads_opt_(NumBuffers)  const UINT *pOffsets) = 0;	//�����µ��� �迭�� ù ���Ҹ� ����Ű�� ������. 
		//���⼭ �������� ���� ������ġ���� ��ǥ�ϴ� ���� ��ġ���� �Ÿ�
	*/
	Module::GetDeviceContext().IASetIndexBuffer(this->indexbuffer.Get(), DXGI_FORMAT::DXGI_FORMAT_R32_UINT, 0);
	Module::GetDeviceContext().DrawIndexed(this->indexbuffer.IndexCount(), 0, 0);
	/*
	virtual void STDMETHODCALLTYPE DrawIndexed( 
		_In_  UINT IndexCount,				//�� �׸��� ȣ�⿡�� ����� �ε��� ����
		_In_  UINT StartIndexLocation,		//����� ù ������ ��ġ
		_In_  INT BaseVertexLocation) = 0;	//�������� �������� ����, �� �׸��� ȣ�⿡�� ����� ���ε鿡 �������� ���� ��
	*/

}

void Mesh::Draw_SkinnedMesh()
{
	UINT offset = 0;

	for (int i = 0; i < textures.size(); i++) {//���� ���߿� ����ȭ
		if (textures[i]->GetType() == aiTextureType::aiTextureType_DIFFUSE) {
			Module::GetDeviceContext().PSSetShaderResources(i, 1, textures[i]->GetTextureResourceViewAddress()); //�ȼ� ���̴��� �ؽ��� ����
			break;
		}
	}

	Module::GetDeviceContext().IASetVertexBuffers(0, 1, this->vertex_BoneWeight_buffer.GetAddressOf(), this->vertex_BoneWeight_buffer.StridePtr(), &offset);

	Module::GetDeviceContext().IASetIndexBuffer(this->indexbuffer.Get(), DXGI_FORMAT::DXGI_FORMAT_R32_UINT, 0);
	//Module::GetDeviceContext().Draw(1, 0);
	Module::GetDeviceContext().DrawIndexed(this->indexbuffer.IndexCount(), 0, 0);
}

void Mesh::Draw_NoTexture()
{
	UINT offset = 0;

	Module::GetDeviceContext().IASetVertexBuffers(0, 1, this->vertexbuffer.GetAddressOf(), this->vertexbuffer.StridePtr(), &offset);
	Module::GetDeviceContext().IASetIndexBuffer(this->indexbuffer.Get(), DXGI_FORMAT::DXGI_FORMAT_R32_UINT, 0);

	Module::GetDeviceContext().DrawIndexed(this->indexbuffer.IndexCount(), 0, 0);
}

void Mesh::Draw_Billboard()
{
	UINT offset = 0;

	for (int i = 0; i < textures.size(); i++) {//���� ���߿� ����ȭ
		if (textures[i]->GetType() == aiTextureType::aiTextureType_DIFFUSE) {
			Module::GetDeviceContext().PSSetShaderResources(i, 1, textures[i]->GetTextureResourceViewAddress()); //�ȼ� ���̴��� �ؽ��� ����
			break;
		}
	}

	Module::GetDeviceContext().IASetVertexBuffers(0, 1, this->vertex_BoneWeight_buffer.GetAddressOf(), this->vertex_BoneWeight_buffer.StridePtr(), &offset);
	Module::GetDeviceContext().IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY::D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);
	Module::GetDeviceContext().Draw(3, 0);

	Module::GetDeviceContext().IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY::D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

const DirectX::XMMATRIX & Mesh::GetTransformMatrix()
{
	return this->transformMatrix;
}

Mesh::Mesh(std::vector<Texture*> & textures,
	const DirectX::XMMATRIX & transformMatrix)
{
	this->textures = textures;
	this->transformMatrix = transformMatrix;
}

#pragma region SkinnedMesh
//SkinnedMesh::SkinnedMesh(ID3D11Device * _device, ID3D11DeviceContext * _deviceContext, 
//	std::vector<Vertex3D>& _vertices, 
//	std::vector<DWORD>& _indices, 
//	std::vector<Texture>& _textures, 
//	const DirectX::XMMATRIX & _transformMatrix)
//	: Mesh(_deviceContext, _textures, _transformMatrix)
//{
//	try {
//		HRESULT hr = this->vertexbuffer.Initialize_DynamicBuffer(_device, _vertices.data(), _vertices.size());
//		COM_ERROR_IF_FAILED(hr, "Failed to initialize vertex buffer for mesh.");
//
//		hr = this->indexbuffer.Initialize(_device, _indices.data(), _indices.size());
//		COM_ERROR_IF_FAILED(hr, "Failed to initialize index buffer for mesh.");
//	}
//	catch (COMException e) {
//		ErrorLogger::Log(e);
//		return;
//	}
//}
//
//SkinnedMesh::SkinnedMesh(ID3D11Device * _device, ID3D11DeviceContext * _deviceContext, 
//	Vertex3D * _vertices, const UINT _vertexSize, 
//	DWORD * _indices, const UINT _indexSize, 
//	std::vector<Texture>& _textures, 
//	const DirectX::XMMATRIX & _transformMatrix)
//	: Mesh(_deviceContext, _textures, _transformMatrix)
//{
//	try {
//		HRESULT hr = this->vertexbuffer.Initialize_DynamicBuffer(_device, _vertices, _vertexSize);
//		COM_ERROR_IF_FAILED(hr, "Failed to initialize vertex buffer for mesh.");
//
//		hr = this->indexbuffer.Initialize(_device, _indices, _indexSize);
//		COM_ERROR_IF_FAILED(hr, "Failed to initialize index buffer for mesh.");
//	}
//	catch (COMException e) {
//		ErrorLogger::Log(e);
//		return;
//	}
//}
//
//void SkinnedMesh::Draw()
//{
//	UINT offset = 0;
//
//	for (int i = 0; i < textures.size(); i++) {//���� ���߿� ����ȭ
//		if (textures[i].GetType() == aiTextureType::aiTextureType_DIFFUSE) {
//			Module::GetDeviceContext().PSSetShaderResources(i, 1, textures[i].GetTextureResourceViewAddress()); //�ȼ� ���̴��� �ؽ��� ����
//			break;
//		}
//	}
//
//	Module::GetDeviceContext().IASetVertexBuffers(0, 1, this->vertexbuffer.GetAddressOf(), this->vertexbuffer.StridePtr(), &offset);
//
//	Module::GetDeviceContext().IASetIndexBuffer(this->indexbuffer.Get(), DXGI_FORMAT::DXGI_FORMAT_R32_UINT, 0);
//	Module::GetDeviceContext().DrawIndexed(this->indexbuffer.IndexCount(), 0, 0);
//}

#pragma endregion

DirectX::XMMATRIX MyCustom::GetAiMatrixData(aiMatrix4x4 & pSource)
{
	return DirectX::XMMatrixSet(
		pSource.a1, pSource.a2, pSource.a3, pSource.a4,
		pSource.b1, pSource.b2, pSource.b3, pSource.b4,
		pSource.c1, pSource.c2, pSource.c3, pSource.c4,
		pSource.d1, pSource.d2, pSource.d3, pSource.d4);
}

void MyCustom::Vertex_Bone_Data::AddBoneData(int pBoneID, float pWeight)
{
	for (int i = 0; i < NUM_BONES_PER_VERTEX; i++) {
		if (BoneWeights[i] == 0.0) {
			BoneIDs[i] = pBoneID;
			BoneWeights[i] = pWeight;
			return;
		}
	}

	// should never get here - more bones than we have space for
	assert("Bones exceed more than 4 " && 0);
}
