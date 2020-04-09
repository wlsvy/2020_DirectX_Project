#include "Model.h"
#include "../../Util/StringHelper.h"

bool Model::Initialize(std::vector<Mesh>&& meshes)
{
	m_Meshes = std::forward<std::vector<Mesh>>(meshes);
	return true;
}