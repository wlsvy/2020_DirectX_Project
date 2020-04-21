#include "Model.h"
#include "../../Util/StringHelper.h"

bool Model::Initialize(const std::vector<std::shared_ptr<MeshBase>>& meshes)
{
	m_Meshes = meshes;
	return true;
}

bool SkinnedModel::Initialize(
	const std::vector<std::shared_ptr<MeshBase>> & meshes,
	const std::vector<DirectX::XMMATRIX> & boneMatrices,
	const std::unordered_map<std::string, UINT> & boneIdMap)
{
	m_Meshes = meshes;
	m_BoneOffsets = boneMatrices;
	m_BoneIdMap = boneIdMap;
	return true;
}