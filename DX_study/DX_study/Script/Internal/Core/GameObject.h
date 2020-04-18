#pragma once
#include <vector>
#include <memory>
#include "ObjectPool.h"
#include "Scene.h"
#include "../../Component/Component.h"

class Component;
class Transform;
class Renderable;

class GameObject : public Object{
	friend class Scene;
public:
	MANAGED_OBJECT(GameObject)
	explicit GameObject();
	explicit GameObject(const std::string & name);
	virtual ~GameObject();

	template<typename T>
	std::shared_ptr<T> AddComponent();
	template<typename T>
	std::shared_ptr<T> GetComponent();
	void RemoveExpiredComponent();

	Transform& GetTransform() { return *m_Transform; }
	std::shared_ptr<Transform> GetTransformPtr() { return m_Transform; }
	Renderable& GetRenderer() { return *m_Renderer; }

	void OnGui();

protected:

	std::shared_ptr<Transform> m_Transform;
	std::shared_ptr<Renderable> m_Renderer;
	std::vector<std::weak_ptr<Component>> m_Components;
};

template<typename T>
inline std::shared_ptr<T> GameObject::AddComponent()
{
	auto ptr = Pool::CreateInstance<T>(this);
	m_Components.emplace_back(ptr);

	if (Scene::IsInitialized()) {
		ptr->Awake();
	}

	return ptr;
}

template<typename T>
inline std::shared_ptr<T> GameObject::GetComponent()
{
	for (auto& ptr : m_Components) {
		if (auto target = std::dynamic_pointer_cast<T>(ptr.lock())) {
			return target;
		}
	}

	return std::shared_ptr<T>();
}