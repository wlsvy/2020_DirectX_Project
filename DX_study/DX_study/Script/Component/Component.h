#pragma once
#include "../Internal/Core/Object.h"
#include <memory>

class GameObject;

class Component abstract : public Object {
public:
	Component(GameObject* gameObj);
	GameObject* GetGameObject();

	bool Enable = true;

protected:
	GameObject * const m_GameObject;
};