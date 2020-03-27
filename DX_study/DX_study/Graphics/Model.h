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
	//애니메이션 포함 파일
	bool Initialize(
		const std::string & filePath, 
		std::vector<AnimationClip> * _animClipDestination,
		std::map<std::string, int> & _textureMap,
		std::vector<Texture> & _textureVec);

	//일반 파일
	bool Initialize(const std::string & filePath, 
		std::map<std::string, int> & _textureMap,
		std::vector<Texture> & _textureVec);

	////------------------직접 만든 정점/색인 배열로 모델 생성
	bool Initialize(std::vector<Vertex3D> * VertexBuffer, 
		std::vector<DWORD> * IndexBuffer, 
		std::map<std::string, int> & _textureMap,
		std::vector<Texture> & _textureVec);

	bool Initialize(Vertex3D * _VertexBuffer, 
		const UINT _vertexSize, 
		DWORD * _IndexBuffer, 
		const UINT _indexSize, 
		std::map<std::string, int> & _textureMap,
		std::vector<Texture> & _textureVec);

	////------------------빌보드용 모델 생성
	bool Initialize(Vertex3D * _VertexBuffer, 
		const UINT _vertexSize, 
		std::map<std::string, int> & _textureMap,
		std::vector<Texture> & _textureVec);
	
	void(*DRAW_MODEL) ();
	void Draw(const DirectX::XMMATRIX & worldMatrix, const DirectX::XMMATRIX & viewProjectionMatrix);
	void Draw_WireFrame(const DirectX::XMMATRIX & worldMatrix, const DirectX::XMMATRIX & viewProjectionMatrix);
	void Draw_skinnedMesh(const DirectX::XMMATRIX & worldMatrix, const DirectX::XMMATRIX & viewProjectionMatrix, Animator * _animator);
	void Draw_BillBoard(const DirectX::XMMATRIX & _worldMatrix, const DirectX::XMMATRIX & _viewProjectionMatrix, float _height, float _width);

protected:
	
	bool LoadModel(const std::string & filePath,
		std::map<std::string, int> & _textureMap,
		std::vector<Texture> & _textureVec);
	bool LoadModel(const std::string & filePath, 
		std::vector<AnimationClip> * _animClipDestination,
		std::map<std::string, int> & _textureMap,
		std::vector<Texture> & _textureVec);
	void ProcessNode(aiNode * node, 
		const aiScene * scene, 
		const DirectX::XMMATRIX & parentTransformMatrix,
		std::map<std::string, int> & _textureMap,
		std::vector<Texture> & _textureVec);

	Mesh ProcessMesh(aiMesh * mesh,
		const aiScene * scene,
		const DirectX::XMMATRIX & transformMatrix,
		std::map<std::string, int>& _textureMap,
		std::vector<Texture>& _textureVec);
	Mesh ProcessMesh(std::vector<Vertex3D> * _vertexBuffer, std::vector<DWORD> * _indexBuffer, std::map<std::string, int> & _textureMap, std::vector<Texture> & _textureVec);
	Mesh ProcessMesh(std::vector<Vertex3D_BoneWeight>* _vertexBuffer, std::vector<DWORD>* _indexBuffer, std::map<std::string, int> & _textureMap, std::vector<Texture> & _textureVec);
	Mesh ProcessMesh(Vertex3D * _vertexBuffer, const int _vertexSize, DWORD * _indexBuffer, const int _indexSize, std::map<std::string, int> & _textureMap, std::vector<Texture> & _textureVec);
	Mesh ProcessMesh(Vertex3D * _vertex, std::map<std::string, int> & _textureMap, std::vector<Texture> & _textureVec); //점 하나
	void ProcessAnimation(aiAnimation * _aiAnim, const aiScene * _aiScene, std::vector<AnimationClip> * _animClipDestination);
	void ProcessBoneHierarchy(aiNode * _aiNode, AnimationClip * _animClip, BoneChannel * _parentBone, const DirectX::XMMATRIX & _parentTransform);
	TextureStorageType DetermineTextureStorageType(const aiScene* pScene, aiMaterial* pMat, unsigned int index, aiTextureType textureType);
	Mesh ProcessTerrain(std::vector<Vertex3D> * terrainVertexBuffer, std::vector<DWORD> * terrainIndexBuffer);


	std::vector<Texture*> LoadMaterialTextures(
		aiMaterial * pMaterial,
		aiTextureType textureType,
		const aiScene * pScene,
		std::map<std::string, int> & _textureMap,
		std::vector<Texture> & _TextureVec);

	int GetTextureIndex(aiString * pStr);

	std::vector<Mesh> meshes;
	std::string mName = "";
	std::string directory = "";

#pragma region Animation Variable
	std::map<std::string, UINT> m_Bone_Name_Map;
	std::vector<MyCustom::Bone> mBoneBuffer;
	UINT mBoneCount = 0;

	void nodeName(std::vector<std::string> * na, aiNode* node);
#pragma endregion
};

class SkinnedModel : public Model {
	friend class GraphicsManager;
public:

private:

};