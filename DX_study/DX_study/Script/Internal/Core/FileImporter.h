#pragma once
#include <string>
#include <vector>
#include <DirectXMath.h>
#include <unordered_map>

#include "../Graphics/Texture.h"
#include "../Graphics/Mesh.h"
#include "../Graphics/Material.h"

struct aiScene;
struct aiNode;
struct aiMesh;
struct aiMaterial;
struct aiAnimation;
struct BoneChannel;
class AnimationClip;
class SkinnedModel;

class ModelImporterBase {
public:
	virtual bool CreateModel(const std::string & dirPath,
		const std::string & fileName,
		const aiScene * scene) = 0;
protected:
	virtual void ProcessMesh(aiMesh * mesh, const aiScene * scene, const DirectX::XMMATRIX & transformMatrix) = 0;

	void ProcessNode(aiNode * node, const aiScene * scene, const DirectX::XMMATRIX & parentTransformMatrix);
	TextureStorageType DetermineTextureStorageType(const aiScene* pScene, aiMaterial* pMat, unsigned int index, aiTextureType textureType);
	std::shared_ptr<SharedMaterial> LoadMaterial(aiMaterial * pMaterial, aiTextureType textureType, const aiScene * pScene);
	int GetTextureIndex(aiString * pStr);

	std::vector<std::shared_ptr<MeshBase>> m_Meshes;
	std::vector<std::shared_ptr<SharedMaterial>> m_Materials;
	std::string m_Directory;
};

class ModelImporter : public ModelImporterBase {
public:
	bool CreateModel(const std::string & dirPath,
		const std::string & fileName,
		const aiScene * scene) override;

private:
	void ProcessMesh(aiMesh * mesh, const aiScene * scene, const DirectX::XMMATRIX & transformMatrix) override;
};

class SkinnedModelImporter : public ModelImporterBase {
public:
	bool CreateModel(const std::string & dirPath,
		const std::string & fileName,
		const aiScene * scene) override;

private:
	void ProcessMesh(aiMesh * mesh, const aiScene * scene, const DirectX::XMMATRIX & transformMatrix) override;

	struct Bone {
		DirectX::XMMATRIX BoneOffset;
	};
	struct Vertex_Bone_Data {
		const static int MAX_BONE_PER_VERTEX = 4;

		int BoneIDs[MAX_BONE_PER_VERTEX] = { -1, -1, -1, -1 };
		float BoneWeights[MAX_BONE_PER_VERTEX] = { 0.0f, 0.0f, 0.0f, 0.0f };
	};
	std::unordered_map<std::string, UINT> m_BoneIdMap;
	std::vector<DirectX::XMMATRIX> m_BoneOffsets;
};

class AnimationImporter {
public:
	void LoadAnimation(const std::string & name, const std::shared_ptr<SkinnedModel> & baseModel, const aiScene * scene);

private:
	void ProcessAnimation(const std::string & name, aiAnimation * anim, const aiScene * scene);
	void ProcessBoneHierarchy(aiNode * node, AnimationClip * animClip, BoneChannel * parentBone, const DirectX::XMMATRIX & parentTransform);

	struct Bone {
		DirectX::XMMATRIX BoneOffset;
		DirectX::XMMATRIX FinalTransform;
	};
	struct Vertex_Bone_Data {
		const static int MAX_BONE_PER_VERTEX = 4;

		int BoneIDs[MAX_BONE_PER_VERTEX] = { -1, -1, -1, -1 };
		float BoneWeights[MAX_BONE_PER_VERTEX] = { 0.0f, 0.0f, 0.0f, 0.0f };
	};

	std::shared_ptr<SkinnedModel> m_BaseModel;
};

class CsvImporter {
public:
	using ColumnMap = std::unordered_map<std::string, std::vector<std::string>>;
	ColumnMap LoadCSV(const std::string & filePath);
};