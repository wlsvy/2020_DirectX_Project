#pragma once
#include "Vertex.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "ConstantBuffer.h"
#include "Texture.h"

#define NUM_BONES_PER_VERTEX 4

namespace MyCustom {
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
	enum DrawFlag
	{
		Default = 0,
		SkinnedMesh = 1,
		NoTexture = 2,
		BillBoard = 3
	};

	Mesh(
		const std::vector<Vertex3D> & vertices, 
		const std::vector<DWORD> & indices, 
		const std::vector<Texture*> & textures, 
		const DirectX::XMMATRIX & transformMatrix,
		const DrawFlag _drawflag);

	//skinned mesh Àü¿ë
	Mesh(
		std::vector<Vertex3D_BoneWeight> & _vert_bones,
		std::vector<DWORD> & indices,
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
	Mesh(
		std::vector<Texture*> & textures,
		const DirectX::XMMATRIX & transformMatrix);

	VertexBuffer<Vertex3D> vertexbuffer;
	VertexBuffer<Vertex3D_BoneWeight> vertex_BoneWeight_buffer;
	IndexBuffer indexbuffer;

	DirectX::XMMATRIX transformMatrix;

	bool hasBone = false;
	DrawFlag m_DrawFlag = Default;
};