#include "Material.h"
#include "../Core/ObjectPool.h"

std::shared_ptr<SharedMaterial> SharedMaterial::GetDefault() {
	return Pool::Find<SharedMaterial>("Default");
}