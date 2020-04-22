#pragma once
#include <unordered_map>
#include "Mesh.h"
#include "Material.h"
#include "../Core/Object.h"

class SharedMaterial;

class Model : public Object {
	MANAGED_OBJECT(Model)
public:
	Model(
		const std::vector<std::shared_ptr<MeshBase>>& meshes,
		const std::vector<std::shared_ptr<SharedMaterial>> & defaultMats,
		const std::string & name = "Model") : 
		Object(name), 
		m_Meshes(meshes),
		m_DefaultMaterial(defaultMats) {}

	const std::vector<std::shared_ptr<MeshBase>> & GetMeshes() const { return m_Meshes; }
	const std::vector<std::shared_ptr<SharedMaterial>> & GetDefaultMaterials() const { return m_DefaultMaterial; }

private:
	std::vector<std::shared_ptr<MeshBase>> m_Meshes;
	std::vector<std::shared_ptr<SharedMaterial>> m_DefaultMaterial;
};

class SkinnedModel : public Model {
public:
	SkinnedModel(
		const std::vector<std::shared_ptr<MeshBase>>& meshes,
		const std::vector<std::shared_ptr<SharedMaterial>> & defaultMats,
		const std::vector<DirectX::XMMATRIX> & boneMatrices,
		const std::unordered_map<std::string, UINT> & boneIdMap,
		const std::string & name = "Model") :
		Model(meshes, defaultMats, name),
		m_BoneIdMap(boneIdMap),
		m_BoneOffsets(boneMatrices) {}

	const std::unordered_map<std::string, UINT> & GetBoneIdMap() const	{ return m_BoneIdMap; }
	const std::vector<DirectX::XMMATRIX> & GetBoneOffsets() const		{ return m_BoneOffsets; }

private:
	std::unordered_map<std::string, UINT> m_BoneIdMap;
	std::vector<DirectX::XMMATRIX> m_BoneOffsets;
};