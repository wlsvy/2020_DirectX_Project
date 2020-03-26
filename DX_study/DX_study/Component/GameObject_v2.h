#pragma once
#include <queue>
#include <vector>
#include <memory>

#include "Component.h"
#include "../Engine/SceneManager.h"

class VertexShader;
class PixelShader;
class GeometryShader;
struct ID3D11DeviceContext;

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
	GameObject_v2(SceneManager * const sceneM,  Model * model, const int & vshaderID, const int & pshaderID, const DirectX::XMFLOAT3 & pos, const DirectX::XMFLOAT3 & rot);
	GameObject_v2(const GAMEOBJECT_INIT_DESC & desc);
	~GameObject_v2();
	int getID();
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

	char mGameObjectName[30];

	bool enabled = true;
	Transform transform;
	Renderer renderer;
private:
	std::vector<std::shared_ptr<Component>> m_Components;
	SceneManager * const sceneManager;

	int mGameObjectID;
};

template<class T>
inline T* GameObject_v2::AddComponent()
{
	sceneManager->Component_Valid_Test();

	typedef std::vector<std::shared_ptr<Component>>::iterator Component_Iterator;
	Component_Iterator begin = m_Components.begin();
	Component_Iterator end = m_Components.end();

	//이미 해당 컴포넌트를 가지고 있는지 확인
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

	T* new_Component = new T(compo_desc);
	Component* compo_ptr = dynamic_cast<Component*>(new_Component);
	sceneManager->ClassifyComponent(compo_ptr, this);
	
	return new_Component;
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
			sceneManager->Component_Valid_Test();
			return;
		}

		iter++;
	}

	//없는 경우
	MessageBoxA(NULL, "No Component To Remove", "Error", MB_ICONERROR);
	return;
}


