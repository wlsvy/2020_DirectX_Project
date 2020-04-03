#include "Component.h"

GameObject* Component::GetGameObject()
{
	if (auto ptr = m_GameObject.lock()) {
		return ptr.get();
	}
	return nullptr;
}
