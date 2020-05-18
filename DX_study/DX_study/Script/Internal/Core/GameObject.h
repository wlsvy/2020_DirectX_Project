#pragma once
#include <vector>
#include <memory>
#include "ObjectPool.h"
#include "Scene.h"
#include "../../Component/Component.h"

class Component;
class Transform;
class RenderInfo;
class Model;

class GameObject : public Object{
	friend class Scene;
public:
	MANAGED_OBJECT(GameObject)
	GameObject(const std::string & name = "GameObject");
	virtual ~GameObject() {}

	template<typename T>
	std::shared_ptr<T> AddComponent();
	template<typename T>
	std::shared_ptr<T> GetComponent();
	void RemoveExpiredComponent();

	Transform& GetTransform() { return *m_Transform; }
	RenderInfo& GetRendererable() { return *m_Renderer; }
	std::shared_ptr<Transform> GetTransformPtr() { return m_Transform; }
	std::shared_ptr<RenderInfo> GetRenderablePtr() { return m_Renderer; }

	void OnGui(const char * option = "op") override;

protected:

	std::shared_ptr<Transform> m_Transform;
	std::shared_ptr<RenderInfo> m_Renderer;
	std::vector<std::weak_ptr<Component>> m_Components;
};

template<typename T>
inline std::shared_ptr<T> GameObject::AddComponent()
{
	auto ptr = Core::CreateInstance<T>(this);
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