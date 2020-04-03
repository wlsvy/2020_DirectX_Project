#pragma once
#include "../Internal/Core/Object.h"
#include <memory>

class GameObject;

class Component abstract : public Object {
public:
	GameObject* GetGameObject();

private:
	std::weak_ptr<GameObject> m_GameObject;
};