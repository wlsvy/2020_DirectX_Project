#include "GameObject.h"
#include "../Component/Transform.h"
#include "../Internal/Core/ObjectPool.h"

GameObject::GameObject() : m_Transform(std::make_shared<Transform>(this))
{
	m_Transform->SetParent(Core::GetWorldTransform());
}

GameObject::~GameObject()
{
}

Transform & GameObject::GetTransform()
{
	return *m_Transform.get();
}

