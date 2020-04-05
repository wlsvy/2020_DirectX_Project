#pragma once
#include "../Internal/Core/Object.h"
#include <memory>

class GameObject;

#define COMPONENT_CONSTRUCTOR(type) \
public: \
explicit type(GameObject* gameObj) : Component(gameObj, #type) {} \
explicit type(GameObject * gameObj, const std::string & name) : Component(gameObj, name) {} \

class Component abstract : public Object {
public:
	Component(GameObject* gameObj);
	Component(GameObject* gameObj, const std::string& name);
	GameObject* GetGameObject();

protected:
	GameObject * const m_GameObject;
};