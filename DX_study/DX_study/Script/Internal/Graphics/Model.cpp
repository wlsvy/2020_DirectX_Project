#include "Model.h"
#include "../Core/ObjectPool.h"

Model::Model(const std::shared_ptr<MeshBase>& mesh, const std::string & name) : Object(name)
{
	m_Meshes.push_back(mesh);
	m_DefaultMaterial.push_back(Core::Find<SharedMaterial>("Default"));
}
