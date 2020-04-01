#pragma once
#include "Mesh.h"
#include "../Core/Object.h"

using namespace DirectX;

class Model : public Object {
public:
	MANAGED_OBJECT(Model)

	bool Initialize(const std::string & filePath);
	void Draw(const DirectX::XMMATRIX & worldMatrix, const DirectX::XMMATRIX & viewProjectionMatrix);

private:
	bool LoadModel(const std::string & filePath);
	void ProcessNode(aiNode * node, const aiScene * scene, const XMMATRIX & parentTransformMatrix);
	Mesh ProcessMesh(aiMesh * mesh, const aiScene * scene, const XMMATRIX & transformMatrix);
	TextureStorageType DetermineTextureStorageType(const aiScene* pScene, aiMaterial* pMat, unsigned int index, aiTextureType textureType);

	std::vector<Texture> LoadMaterialTextures(aiMaterial* pMaterial, aiTextureType textureType, const aiScene* pScene);
	int GetTextureIndex(aiString * pStr);

	std::vector<Mesh> m_Meshes;
	std::string m_Directory = "";
};