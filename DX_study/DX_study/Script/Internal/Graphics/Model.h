#pragma once
#include "Mesh.h"
#include <unordered_map>
#include "../Core/Object.h"

class Model : public Object {
	MANAGED_OBJECT(Model)
public:
	Model(
		const std::vector<std::shared_ptr<MeshBase>>& meshes, 
		const std::string & name = "Model") : 
		Object(name), 
		m_Meshes(meshes) {}

	const std::vector<std::shared_ptr<MeshBase>> & GetMeshes() const { return m_Meshes; }

private:
	std::vector<std::shared_ptr<MeshBase>> m_Meshes;
};

class SkinnedModel : public Model {
	MANAGED_OBJECT(SkinnedModel)
public:
	SkinnedModel(
		const std::vector<std::shared_ptr<MeshBase>>& meshes,
		const std::vector<DirectX::XMMATRIX> & boneMatrices,
		const std::unordered_map<std::string, UINT> & boneIdMap,
		const std::string & name = "Model") :
		Model(meshes, name),
		m_BoneIdMap(boneIdMap),
		m_BoneOffsets(boneMatrices) {}

	const std::unordered_map<std::string, UINT> & GetBoneIdMap() const	{ return m_BoneIdMap; }
	const std::vector<DirectX::XMMATRIX> & GetBoneOffsets() const		{ return m_BoneOffsets; }

private:
	std::unordered_map<std::string, UINT> m_BoneIdMap;
	std::vector<DirectX::XMMATRIX> m_BoneOffsets;
};