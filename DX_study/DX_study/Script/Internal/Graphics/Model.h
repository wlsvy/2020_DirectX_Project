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
		const std::vector<std::shared_ptr<ShaderState>> & defaultShaderState,
		const std::string & name = "Model") : 
		Object(name), 
		m_Meshes(meshes),
		m_DefaultMaterial(defaultMats), 
		m_DefulatShaderState(defaultShaderState) {}
	Model(const std::shared_ptr<Mesh> & mesh, const std::string & name) : 
		Object(name), 
		m_Meshes(1, mesh), 
		m_DefaultMaterial(1, SharedMaterial::GetDefault()),
		m_DefulatShaderState(1, ShaderState::GetDefault()) {}

	const std::vector<std::shared_ptr<Mesh>> & GetMeshes() const						{ return m_Meshes; }
	const std::vector<std::shared_ptr<SharedMaterial>> & GetDefaultMaterials() const	{ return m_DefaultMaterial; }
	const std::vector<std::shared_ptr<ShaderState>> & GetDefaultShaderState() const		{ return m_DefulatShaderState; }

private:
	std::vector<std::shared_ptr<Mesh>> m_Meshes;
	std::vector<std::shared_ptr<SharedMaterial>> m_DefaultMaterial;
	std::vector<std::shared_ptr<ShaderState>> m_DefulatShaderState;
};

class SkinnedModel : public Model {
public:
	SkinnedModel(
		const std::vector<std::shared_ptr<Mesh>>& meshes,
		const std::vector<std::shared_ptr<SharedMaterial>> & defaultMats,
		const std::vector<std::shared_ptr<ShaderState>> & defaultShaderState,
		const std::vector<DirectX::XMMATRIX> & boneMatrices,
		const std::unordered_map<std::string, UINT> & boneIdMap,
		const std::string & name = "Model") :
		Model(meshes, defaultMats, defaultShaderState, name),
		m_BoneIdMap(boneIdMap),
		m_BoneOffsets(boneMatrices) {}

	const std::unordered_map<std::string, UINT> & GetBoneIdMap() const	{ return m_BoneIdMap; }
	const std::vector<DirectX::XMMATRIX> & GetBoneOffsets() const		{ return m_BoneOffsets; }

private:
	std::unordered_map<std::string, UINT> m_BoneIdMap;
	std::vector<DirectX::XMMATRIX> m_BoneOffsets;
};