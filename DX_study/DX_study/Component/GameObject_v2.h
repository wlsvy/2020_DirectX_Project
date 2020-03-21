#pragma once
#include <queue>
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

	std::shared_ptr<T> component(new T(*this));
	//Module::RegisterComponent<T>(component);
	Module::RegisterCompo<T>(component);
	this->m_Components.emplace_back(component);
	return component.get();
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
	for (auto& ptr : m_Components) {
		T *target = dynamic_cast<T*>(ptr.get());
		if (target != nullptr) {
			m_Components.erase(ptr);
			sceneManager->Component_Valid_Test();
			return;
		}
	}

	//없는 경우
	MessageBoxA(NULL, "No Component To Remove", "Error", MB_ICONERROR);
	return;
}


