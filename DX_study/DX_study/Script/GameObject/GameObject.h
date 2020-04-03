#pragma once
#include "../Internal/Graphics/Model.h"
#include "../Internal/Core/Object.h"

class Component;
class Transform;
class Renderer;

class GameObject : public Object{
public:
	MANAGED_OBJECT(GameObject)
	GameObject();
	~GameObject();

	Transform& GetTransform();

protected:
	std::shared_ptr<Transform> m_Transform;
	std::shared_ptr<Renderer> m_Renderer;
	std::vector<std::shared_ptr<Component>> m_Components;
};