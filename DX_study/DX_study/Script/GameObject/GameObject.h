#pragma once
#include "../Internal/Graphics/Model.h"
#include "../Internal/Core/Object.h"

class Component;
class Transform;
class Renderable;

class GameObject : public Object{
public:
	MANAGED_OBJECT(GameObject)
	explicit GameObject();
	explicit GameObject(const std::string & name);
	~GameObject();

	Transform& GetTransform() { return *m_Transform; }
	std::shared_ptr<Transform> GetTransformPtr() { return m_Transform; }
	Renderable& GetRenderer() { return *m_Renderer; }

protected:
	std::shared_ptr<Transform> m_Transform;
	std::shared_ptr<Renderable> m_Renderer;
	std::vector<std::shared_ptr<Component>> m_Components;
};