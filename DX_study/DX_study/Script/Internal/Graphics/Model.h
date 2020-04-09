#pragma once
#include "Mesh.h"
#include "../Core/Object.h"

class Model : public Object {
	MANAGED_OBJECT(Model)
public:
	bool Initialize(std::vector<Mesh>&& meshes);
	const std::vector<Mesh>& GetMeshes() const { return m_Meshes; }

private:
	std::vector<Mesh> m_Meshes;
};