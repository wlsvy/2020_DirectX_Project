#pragma once
#include <vector>
#include <memory>
#include <unordered_map>

#include "../../Util/Singleton.h"
#include "Object.h"

template<typename T>
class ObjectPool :  public Singleton<ObjectPool<T>> {
public:
	void Register(const std::shared_ptr<T>& obj) { 
		m_Objects.push_back(obj); 
	}

	void DeRegister(std::shared_ptr<T> obj) {
		auto iter = m_Objects.find(m_Objects.begin().m_Objects.end(), obj);
		if (iter != m_Objects.end()) {
			m_Objects.erase(iter);
		}
	}

private:
	std::vector<std::shared_ptr<T>> m_Objects;
};

template<>
class ObjectPool<Object> : public Singleton<ObjectPool<Object>> {
public:
	template<typename T>
	void Register(const std::shared_ptr<T>& obj) {
		m_Objects.insert(make_pair(obj->GetId(), std::make_unique<ObjectWrapper<T>>(obj)));
	}

	void DeRegister(const int objId) {
		auto iter = m_Objects.find(objId);
		if (iter != m_Objects.end()) {
			m_Objects.erase(iter);
		}
	}
	void DeRegister(const std::shared_ptr<Object>& obj) {
		DeRegister(obj->GetId());
	}

	
	std::shared_ptr<Object> Find(const int objId) {
		auto iter = m_Objects.find(objId);
		if (iter != m_Objects.end()) {
			return iter->second->Ptr;
		}
		return std::shared_ptr<Object>();
	}
private:
	class ObjWrapperBase {
	public:
		std::shared_ptr<Object> Ptr;
	};

	template<typename T>
	class ObjectWrapper : public ObjWrapperBase {
	public:
		ObjectWrapper(const std::shared_ptr<T>& obj) : Ptr(obj) {
			ObjectPool<typename T::Base_Type>::GetInstance().Register(obj);
		}
		~ObjectWrapper() {
			ObjectPool<T>::GetInstance().DeRegister(Ptr);
		}
	};

	std::unordered_map<int, std::shared_ptr<ObjWrapperBase>> m_Objects;
};



template<typename T>
std::shared_ptr<T> CreateInstance() {
	auto ptr = std::make_shared<T>();
	ObjectPool<Object>::GetInstance().Register<T>(ptr);
	return ptr;
}

void Destroy(Object& obj);

//std::unique_ptr<ObjectPool<GameObject>> m_GameObjPool;
//std::unique_ptr<ObjectPool<Shader>> m_ShaderPool;
//std::unique_ptr<ObjectPool<Texture>> m_TexturePool;
//std::unique_ptr<ObjectPool<Model>> m_ModelPool;

//class GameObjectPool : public ObjectPool<GameObject> {
//public:
//
//};
//
//class ShaderPool : public ObjectPool<Shader> {
//public:
//
//};
//
//class TexturePool : public  ObjectPool<Texture> {
//public:
//
//};
//
//class ModelPool : public ObjectPool<Model> {
//public:
//
//};