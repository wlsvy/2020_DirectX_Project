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
	if (m_GameObject) {
		m_GameObject->RemoveExpiredComponent();
	}
}