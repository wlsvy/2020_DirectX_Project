#pragma once
#include <string>
#include <vector>
#include <DirectXMath.h>

#include "../Graphics/Texture.h"
#include "../Graphics/Mesh.h"

class Mesh;
struct aiScene;
struct aiNode;
struct aiMesh;
struct aiMaterial;

class ModelImporter {
public:
	bool LoadModel(const std::string & filePath);

private:
	void ProcessNode(aiNode * node, const aiScene * scene, const DirectX::XMMATRIX & parentTransformMatrix);
	Mesh ProcessMesh(aiMesh * mesh, const aiScene * scene, const DirectX::XMMATRIX & transformMatrix);
	TextureStorageType DetermineTextureStorageType(const aiScene* pScene, aiMaterial* pMat, unsigned int index, aiTextureType textureType);

	std::vector<Texture> LoadMaterialTextures(aiMaterial* pMaterial, aiTextureType textureType, const aiScene* pScene);
	int GetTextureIndex(aiString * pStr);

	std::vector<Mesh> m_Meshes;
	std::string m_Directory;
};

