#pragma once
#include "Mesh.h"
#include <map>

struct ASSIMP_API aiNode;
struct aiScene;
struct aiMesh;
struct aiAnimation;
struct BoneChannel;
class AnimationClip;
class Animator;

class Model {
	friend class GraphicsManager;
public:
	//------------------fbx 파일로 모델 생성
	bool Initialize(const std::string & filePath);

	////------------------직접 만든 정점/색인 배열로 모델 생성
	bool Initialize(std::vector<Vertex3D> * VertexBuffer, 
		std::vector<DWORD> * IndexBuffer);

	bool Initialize(Vertex3D * _VertexBuffer, 
		const UINT _vertexSize, 
		DWORD * _IndexBuffer, 
		const UINT _indexSize);

	////------------------빌보드용 모델 생성
	bool Initialize(Vertex3D * _VertexBuffer, 
		const UINT _vertexSize);
	
	void(*DRAW_MODEL) ();
	void Draw(const DirectX::XMMATRIX & worldMatrix, const DirectX::XMMATRIX & viewProjectionMatrix);
	void Draw_WireFrame(const DirectX::XMMATRIX & worldMatrix, const DirectX::XMMATRIX & viewProjectionMatrix);
	void Draw_skinnedMesh(const DirectX::XMMATRIX & worldMatrix, const DirectX::XMMATRIX & viewProjectionMatrix, Animator * _animator);
	void Draw_BillBoard(const DirectX::XMMATRIX & _worldMatrix, const DirectX::XMMATRIX & _viewProjectionMatrix, float _height, float _width);

protected:
	
	bool LoadModel(const std::string & filePath);
	void ProcessNode(
		aiNode * node, 
		const aiScene * scene, 
		const DirectX::XMMATRIX & parentTransformMatrix);

	Mesh ProcessMesh(aiMesh * mesh,
		const aiScene * scene,
		const DirectX::XMMATRIX & transformMatrix);
	Mesh ProcessMesh(std::vector<Vertex3D> * _vertexBuffer, std::vector<DWORD> * _indexBuffer);
	Mesh ProcessMesh(std::vector<Vertex3D_BoneWeight>* _vertexBuffer, std::vector<DWORD>* _indexBuffer);
	Mesh ProcessMesh(Vertex3D * _vertexBuffer, const int _vertexSize, DWORD * _indexBuffer, const int _indexSize);
	Mesh ProcessMesh(Vertex3D * _vertex); //점 하나
	void ProcessAnimation(aiAnimation * _aiAnim, const aiScene * _aiScene);
	void ProcessBoneHierarchy(aiNode * _aiNode, AnimationClip * _animClip, BoneChannel * _parentBone, const DirectX::XMMATRIX & _parentTransform);
	TextureStorageType DetermineTextureStorageType(const aiScene* pScene, aiMaterial* pMat, unsigned int index, aiTextureType textureType);
	Mesh ProcessTerrain(std::vector<Vertex3D> * terrainVertexBuffer, std::vector<DWORD> * terrainIndexBuffer);


	std::vector<Texture*> LoadMaterialTextures(
		aiMaterial * pMaterial,
		aiTextureType textureType,
		const aiScene * pScene);

	int GetTextureIndex(aiString * pStr);

	std::vector<Mesh> m_Meshes;
	std::string mName = "";
	std::string directory = "";

#pragma region Animation Variable
	std::map<std::string, UINT> m_Bone_Name_Map;
	std::vector<MyCustom::Bone> mBoneBuffer;
	UINT mBoneCount = 0;

	void nodeName(std::vector<std::string> * na, aiNode* node);
#pragma endregion
};