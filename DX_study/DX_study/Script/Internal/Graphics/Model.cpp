#include "Model.h"
#include "../../Util/StringHelper.h"

bool Model::Initialize(const std::vector<Mesh>& meshes)
{
	m_Meshes = meshes;
	return true;
}

bool SkinnedModel::Initialize(
	const std::vector<SkinnedMesh> & meshes,
	const std::vector<DirectX::XMMATRIX> & boneMatrices)
{
	m_Meshes = meshes;
	m_BoneMatrices = boneMatrices;
	return true;
}