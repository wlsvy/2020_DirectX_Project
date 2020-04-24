#include "Material.h"
#include "../Core/ObjectPool.h"

std::shared_ptr<SharedMaterial> SharedMaterial::GetDefault() {
	return Core::Find<SharedMaterial>("Default");
}