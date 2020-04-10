#include "Model.h"
#include "../../Util/StringHelper.h"

bool Model::Initialize(const std::vector<Mesh>& meshes)
{
	m_Meshes = meshes;
	return true;
}

bool SkinnedModel::Initialize(
	const std::vector<SkinnedMesh> & meshes,
	const std::vector<DirectX::XMMATRIX> & boneMatrices,
	const std::unordered_map<std::string, UINT> & boneIdMap)
{
	m_Meshes = meshes;
	m_BoneOffsets = boneMatrices;
	m_BoneIdMap = boneIdMap;
	return true;
}