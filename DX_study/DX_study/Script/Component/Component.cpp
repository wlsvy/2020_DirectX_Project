#include "Component.h"

Component::Component(GameObject * gameObj) : m_GameObject(gameObj)
{
}

GameObject* Component::GetGameObject()
{
	return m_GameObject;
}
