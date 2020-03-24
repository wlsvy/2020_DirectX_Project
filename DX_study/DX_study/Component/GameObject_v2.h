#pragma once
#include <queue>
<<<<<<< HEAD
=======
#include "Object.h"
#include "Component.h"
#include "../SceneManager.h"
>>>>>>> parent of cb3481a... refactoring
#include <vector>
#include <memory>

#include "Component.h"

class VertexShader;
class PixelShader;
class GeometryShader;
class ScriptBehaviour;
class Light_ver2;
class Collider_v2;
class Animator;
class Terrain;

struct ID3D11DeviceContext;

struct GAMEOBJECT_INIT_DESC {
	std::string obj_name = "GameObject";
	int obj_id = -1;

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
	~GameObject_v2();
	bool operator== (const GameObject_v2& rhs) const;

	int getID() const;
	void OnGui();

	template<class T>
	T* AddComponent();
	template<class T>
	T* GetComponent();
	template<class T>
	void RemoveComponent();
	//void CustomSendMessage();
	void Destroy() override;
	void Destroy(GameObject_v2 * _target);

	char Name[30];

	bool enabled = true;
	Transform transform;
	Renderer renderer;
private:
	void RegisterComponent(std::shared_ptr<ScriptBehaviour> compo, ScriptComponentTag);
	void RegisterComponent(std::shared_ptr<Light_ver2> compo, LigthComponentTag);
	void RegisterComponent(std::shared_ptr<Collider_v2> compo, PhysicsComponentTag);
	void RegisterComponent(std::shared_ptr<Terrain> compo, terrainComponentTag);
	void RegisterComponent(std::shared_ptr<Animator> compo, AnimationComponentTag);
	void DeRegisterComponent(std::shared_ptr<ScriptBehaviour> compo, ScriptComponentTag);
	void DeRegisterComponent(std::shared_ptr<Light_ver2> compo, LigthComponentTag);
	void DeRegisterComponent(std::shared_ptr<Collider_v2> compo, PhysicsComponentTag);
	void DeRegisterComponent(std::shared_ptr<Terrain> compo, terrainComponentTag);
	void DeRegisterComponent(std::shared_ptr<Animator> compo, AnimationComponentTag);

	std::vector<std::shared_ptr<Component>> m_Components;

	int mGameObjectID;
};

template<class T>
T* GameObject_v2::AddComponent()
{
	for (auto& ptr : m_Components) {
		T *check = dynamic_cast<T*>(ptr.get());
		if (check != nullptr) {
			assert("Add Component : This component is already exists!");
			return nullptr;
		}
	}

<<<<<<< HEAD
	std::shared_ptr<T> component(new T(*this));

	RegisterComponent(component, typename T::ComponentTag());
	this->m_Components.emplace_back(component);

	return component.get();
=======
	COMPONENT_INIT_DESC compo_desc;
	compo_desc.mGameObj = this;
	compo_desc.mTransform = &transform;
	compo_desc.mTime = sceneManager->Time;
	compo_desc.mRenderer = &renderer;
	compo_desc.mPhysicsManager = sceneManager->getPhysicsManagerPtr();
	compo_desc.mGraphicsManager = sceneManager->getGraphicsManagerPtr();
	compo_desc.mDevice = sceneManager->getDevicePtr();
	compo_desc.mDeviceContext = sceneManager->getDeviceContextPtr();

	T* new_Component = new T(compo_desc);
	Component* compo_ptr = dynamic_cast<Component*>(new_Component);
	sceneManager->ClassifyComponent(compo_ptr, this);
	
	return new_Component;
>>>>>>> parent of cb3481a... refactoring
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
void GameObject_v2::RemoveComponent()
{
	for (auto iter = m_Components.begin(); iter != m_Components.end(); iter++) {
		T *target = dynamic_cast<T*>(iter->get());
		if (target != nullptr) {
			//DeRegisterComponent((*iter), typename T::ComponentTag());
			m_Components.erase(iter);
			return;
		}
	}

	//없는 경우
	MessageBoxA(NULL, "No Component To Remove", "Error", MB_ICONERROR);
}


