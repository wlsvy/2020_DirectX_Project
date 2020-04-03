#include "GameObject.h"
#include "../Component/Transform.h"
#include "../Internal/Core/ObjectPool.h"
#include "../Component/Renderer.h"

GameObject::GameObject() : 
	m_Transform(std::make_shared<Transform>(this)),
	m_Renderer(Pool::CreateInstance<Renderer>(this))
{
	m_Transform->SetParent(Core::GetWorldTransform());
}

GameObject::~GameObject()
{
}

