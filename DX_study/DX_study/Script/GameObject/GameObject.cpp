#include "GameObject.h"
#include "../Component/Transform.h"
#include "../Internal/Core/ObjectPool.h"
#include "../Component/Renderable.h"

GameObject::GameObject() : 
	m_Transform(std::make_shared<Transform>(this)),
	m_Renderer(Pool::CreateInstance<Renderable>(this)),
	Object("GameObject")
{
	m_Transform->SetParent(Core::GetWorldTransform());
}

GameObject::GameObject(const std::string & name) :
	m_Transform(std::make_shared<Transform>(this)),
	m_Renderer(Pool::CreateInstance<Renderable>(this)),
	Object(name)
{
	m_Transform->SetParent(Core::GetWorldTransform());
}

GameObject::~GameObject()
{
}

