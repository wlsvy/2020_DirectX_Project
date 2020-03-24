#pragma once
#include "Vertex.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "ConstantBuffer.h"
#include "Texture.h"

#define NUM_BONES_PER_VERTEX 4

namespace MyCustom {
	//class aiMatrix4x4;
	enum DRAW_FLAG {DEFAULT = 0, SKINNED_MESH = 1, WITHOUT_TEXTURE = 2};

	struct Bone {
		DirectX::XMMATRIX BoneOffset;
		DirectX::XMMATRIX FinalTransform;
	};

	struct Vertex_Bone_Data {
		int BoneIDs[NUM_BONES_PER_VERTEX] = { 0, };
		float BoneWeights[NUM_BONES_PER_VERTEX] = { 0.0f, };

		void AddBoneData(int pBoneID, float pWeight);
	};

	DirectX::XMMATRIX GetAiMatrixData(aiMatrix4x4 & pSource);
}

class Mesh {
public:
	Mesh(ID3D11Device * device, ID3D11DeviceContext * deviceContext,
		std::vector<Vertex3D> & vertices, 
		std::vector<DWORD> & indices, 
		std::vector<Texture*> & textures, 
		const DirectX::XMMATRIX & transformMatrix,
		MyCustom::DRAW_FLAG _drawflag);

	Mesh(ID3D11Device * device, ID3D11DeviceContext * deviceContext, 
		Vertex3D * _vertices, const UINT _vertexSize, 
		DWORD * _indices, const UINT _indexSize, 
		std::vector<Texture*> & textures, 
		const DirectX::XMMATRIX & transformMatrix,
		MyCustom::DRAW_FLAG _drawflag);

	//skinned mesh 전용
	Mesh(ID3D11Device * device, ID3D11DeviceContext * deviceContext,
		std::vector<Vertex3D_BoneWeight> & _vert_bones,
		std::vector<DWORD> & indices,
		std::vector<Texture*> & textures,
		const DirectX::XMMATRIX & transformMatrix);

	//빌보드 메시
	Mesh(ID3D11Device * device, ID3D11DeviceContext * deviceContext,
		Vertex3D & _vertex,
		std::vector<Texture*> & textures,
		const DirectX::XMMATRIX & transformMatrix);

	Mesh(const Mesh & mesh);

	void Draw();
	void (Mesh::*DRAW_MESH) ();
	void Draw_Default();
	void Draw_SkinnedMesh();
	void Draw_NoTexture();
	void Draw_Billboard();
	const DirectX::XMMATRIX & GetTransformMatrix();

	std::vector<Texture*> textures;
protected:
	Mesh(ID3D11DeviceContext * deviceContext,
		std::vector<Texture*> & textures,
		const DirectX::XMMATRIX & transformMatrix);

	VertexBuffer<Vertex3D> vertexbuffer;
	VertexBuffer<Vertex3D_BoneWeight> vertex_BoneWeight_buffer;
	IndexBuffer indexbuffer;

	ID3D11DeviceContext * deviceContext;
	DirectX::XMMATRIX transformMatrix;

	bool hasBone = false;
};

//class SkinnedMesh : public Mesh {
//public:
//	SkinnedMesh(ID3D11Device * _device, ID3D11DeviceContext * _deviceContext,
//		std::vector<Vertex3D> & _vertices, 
//		std::vector<DWORD> & _indices,
//		std::vector<Texture> & _textures,
//		const DirectX::XMMATRIX & _transformMatrix);
//
//	SkinnedMesh(ID3D11Device * _device, ID3D11DeviceContext * _deviceContext, 
//		Vertex3D * _vertices, const UINT _vertexSize, 
//		DWORD * _indices, const UINT _indexSize, 
//		std::vector<Texture>& _textures, 
//		const DirectX::XMMATRIX & _transformMatrix);
//
//	void Draw() override;
//
//private:
//};