#include "Component.h"
#include "../Internal/Core/GameObject.h"

Component::Component(GameObject * gameObj) : 
	m_GameObject(gameObj),
	Object("Component")
{
}

Component::Component(GameObject * gameObj, const std::string & name)
	: m_GameObject(gameObj), 
	Object(name)
{
}

Component::~Component()
{
	m_GameObject->RemoveExpiredComponent();
}

GameObject* Component::GetGameObject()
{
	return m_GameObject;
}
