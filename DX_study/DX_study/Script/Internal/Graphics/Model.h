#pragma once
#include "Mesh.h"
#include <unordered_map>
#include "../Core/Object.h"

class Model : public Object {
	MANAGED_OBJECT(Model)
public:
	bool Initialize(const std::vector<Mesh>& meshes);
	const std::vector<Mesh>& GetMeshes() const { return m_Meshes; }

private:
	std::vector<Mesh> m_Meshes;
};

class SkinnedModel : public Object {
	MANAGED_OBJECT(SkinnedModel)
public:
	bool Initialize(
		const std::vector<SkinnedMesh>& meshes, 
		const std::vector<DirectX::XMMATRIX> & boneMatrices,
		const std::unordered_map<std::string, UINT> & boneIdMap);
	const std::vector<SkinnedMesh>& GetMeshes() const { return m_Meshes; }
	const std::unordered_map<std::string, UINT> & GetBoneIdMap()  { return m_BoneIdMap; }
	const std::vector<DirectX::XMMATRIX> & GetBoneOffsets() const { return m_BoneOffsets; }

private:
	std::vector<SkinnedMesh> m_Meshes;
	std::unordered_map<std::string, UINT> m_BoneIdMap;
	std::vector<DirectX::XMMATRIX> m_BoneOffsets;
};