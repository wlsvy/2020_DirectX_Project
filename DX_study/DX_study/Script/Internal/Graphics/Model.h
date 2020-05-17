#pragma once
#include <unordered_map>

#include "Renderable.h"
#include "Mesh.h"
#include "Material.h"
#include "../Core/Object.h"

class Model : public Object {
	MANAGED_OBJECT(Model)
public:
	Model(
		const std::vector<Renderable> & rendearbles,
		const std::string & name = "Model") :
		Object(name),
		m_DefaultRenderables(rendearbles) {}

	const std::vector<Renderable> GetDefaultRenderables()				{ return m_DefaultRenderables; }
private:
	std::vector<Renderable> m_DefaultRenderables;
};

class SkinnedModel : public Model {
public:
	SkinnedModel(
		const std::vector<Renderable> & rendearbles,
		const std::vector<DirectX::XMMATRIX> & boneMatrices,
		const std::unordered_map<std::string, UINT> & boneIdMap,
		const std::string & name = "Model") :
		Model(rendearbles, name),
		m_BoneIdMap(boneIdMap),
		m_BoneOffsets(boneMatrices) {}

	const std::unordered_map<std::string, UINT> & GetBoneIdMap() const	{ return m_BoneIdMap; }
	const std::vector<DirectX::XMMATRIX> & GetBoneOffsets() const		{ return m_BoneOffsets; }

private:
	std::unordered_map<std::string, UINT> m_BoneIdMap;
	std::vector<DirectX::XMMATRIX> m_BoneOffsets;
};