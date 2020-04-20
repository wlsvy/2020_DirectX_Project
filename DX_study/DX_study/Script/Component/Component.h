#pragma once
#include <memory>
#include "../Internal/Core/Object.h"

class GameObject;

#define COMPONENT_CONSTRUCTOR(type, baseType) \
public: \
explicit type(GameObject* gameObj) : baseType(gameObj, #type) {} \
explicit type(GameObject * gameObj, const std::string & name) : baseType(gameObj, name) {} \

class Component abstract : public Object {
public:
	Component(GameObject* gameObj);
	Component(GameObject* gameObj, const std::string& name);
	virtual ~Component();

	virtual void Awake() {}
	GameObject* GetGameObject() { return m_GameObject; }

protected:
	GameObject * const m_GameObject;
};