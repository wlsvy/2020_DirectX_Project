#pragma once
#include <unordered_map>
#include "Mesh.h"
#include "Material.h"
#include "../Core/Object.h"

class Model : public Object {
	MANAGED_OBJECT(Model)
public:
	Model(
		const std::vector<std::shared_ptr<Mesh>>& meshes,
		const std::vector<std::shared_ptr<SharedMaterial>> & defaultMats,
		const std::string & name = "Model") : 
		Object(name), 
		m_Meshes(meshes),
		m_DefaultMaterial(defaultMats) {}
	Model(const std::shared_ptr<Mesh> & mesh, const std::string & name) : 
		Object(name), 
		m_Meshes(1, mesh), 
		m_DefaultMaterial(1, SharedMaterial::GetDefault()) {}

	const std::vector<std::shared_ptr<Mesh>> & GetMeshes() const { return m_Meshes; }
	const std::vector<std::shared_ptr<SharedMaterial>> & GetDefaultMaterials() const { return m_DefaultMaterial; }

private:
	std::vector<std::shared_ptr<Mesh>> m_Meshes;
	std::vector<std::shared_ptr<SharedMaterial>> m_DefaultMaterial;
};

class SkinnedModel : public Model {
public:
	SkinnedModel(
		const std::vector<std::shared_ptr<Mesh>>& meshes,
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