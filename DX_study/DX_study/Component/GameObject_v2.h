#pragma once
#include <queue>
#include <vector>
#include <memory>

#include "Component.h"

class VertexShader;
class PixelShader;
class GeometryShader;
struct ID3D11DeviceContext;

class ScriptBehaviour;
class Light_ver2;
class Collider_v2;
class Terrain;
class Animator;

struct GAMEOBJECT_INIT_DESC {
	std::string obj_name = "GameObject";
	int obj_id = -1;
	SceneManager * scene_manager = nullptr;

	DirectX::XMFLOAT3 pos = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);
	DirectX::XMFLOAT3 rot = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);
	DirectX::XMFLOAT3 scale = DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f);

	Model * model = nullptr;
	VertexShader*	vshaderPtr = nullptr;
	PixelShader*	pshaderPtr = nullptr;
	GeometryShader* gshaderPtr = nullptr;
};

class GameObject_v2 : public Object {
	friend class Engine;
	friend class SceneManager;
	friend class GraphicsManager;
public:
	GameObject_v2(Model * model, const int & vshaderID, const int & pshaderID, const DirectX::XMFLOAT3 & pos, const DirectX::XMFLOAT3 & rot);
	GameObject_v2(const GAMEOBJECT_INIT_DESC & desc);
	~GameObject_v2() {}
	int getID();
	void OnGui();

	template<class T>
	T* AddComponent();
	template<class T>
	T* GetComponent();
	template<class T>
	void RemoveComponent();

	void Destroy() override;
	void Destroy(GameObject_v2 * _target);

	char mGameObjectName[30];

	bool enabled = true;
	Transform transform;
	Renderer renderer;
private:
	void RegisterComponent(std::shared_ptr<ScriptBehaviour> compo, ScriptComponentTag);
	void RegisterComponent(std::shared_ptr<Light_ver2> compo, LigthComponentTag);
	void RegisterComponent(std::shared_ptr<Collider_v2> compo, PhysicsComponentTag);
	void RegisterComponent(std::shared_ptr<Terrain> compo, TerrainComponentTag);
	void RegisterComponent(std::shared_ptr<Animator> compo, AnimationComponentTag);
	void DeRegisterComponent(std::shared_ptr<ScriptBehaviour> compo, ScriptComponentTag);
	void DeRegisterComponent(std::shared_ptr<Light_ver2> compo, LigthComponentTag);
	void DeRegisterComponent(std::shared_ptr<Collider_v2> compo, PhysicsComponentTag);
	void DeRegisterComponent(std::shared_ptr<Terrain> compo, TerrainComponentTag);
	void DeRegisterComponent(std::shared_ptr<Animator> compo, AnimationComponentTag);

	std::vector<std::shared_ptr<Component>> m_Components;

	int mGameObjectID;
};

template<class T>
inline T* GameObject_v2::AddComponent()
{
	typedef std::vector<std::shared_ptr<Component>>::iterator Component_Iterator;
	Component_Iterator begin = m_Components.begin();
	Component_Iterator end = m_Components.end();

	for (Component_Iterator iter = begin; iter != end;) {
		T *component_tester = dynamic_cast<T*>(iter->get());
		if (component_tester != NULL) {
			assert("Add Component : This component is already exists!");
			return nullptr;
		}

		iter++;
	}

	COMPONENT_INIT_DESC compo_desc;
	compo_desc.mGameObj = this;
	compo_desc.mTransform = &transform;
	compo_desc.mRenderer = &renderer;

	std::shared_ptr<T> ptr(new T(compo_desc));
	RegisterComponent(ptr, typename T::ComponentTag());
	m_Components.emplace_back(ptr);
	
	return ptr.get();
}

template<class T>
T* GameObject_v2::GetComponent()
{
	for (auto& ptr : m_Components) {
		T *target = dynamic_cast<T*>(ptr.get());
		if (target != nullptr) {
			return target;
		}
	}

	return nullptr;
}

template<class T>
inline void GameObject_v2::RemoveComponent()
{
	typedef std::vector<std::shared_ptr<Component>>::iterator Component_Iterator;
	Component_Iterator begin = m_Components.begin();
	Component_Iterator end = m_Components.end();

	//해당 컴포넌트를 가지고 있는지 확인
	for (Component_Iterator iter = begin; iter != end;) {
		T *target_component = dynamic_cast<T*>(iter->get());
		if (target_component != NULL) {
			m_Components.erase(iter);
			return;
		}

		iter++;
	}

	//없는 경우
	MessageBoxA(NULL, "No Component To Remove", "Error", MB_ICONERROR);
	return;
}


