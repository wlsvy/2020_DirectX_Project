#include "GameObject.h"

#include <algorithm>
#include "ObjectPool.h"
#include "InternalHelper.h"
#include "../../Component/Transform.h"
#include "../../Component/Renderable.h"

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

void GameObject::RemoveExpiredComponent()
{
	if (m_Components.size() == 0) return;
	std::remove_if(m_Components.begin(), m_Components.end(), 
		[](std::weak_ptr<Component>& ptr)
	{
		return ptr.expired();
	});
}

