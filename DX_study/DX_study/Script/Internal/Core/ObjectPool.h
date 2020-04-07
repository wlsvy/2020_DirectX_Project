#pragma once
#include <vector>
#include <memory>
#include <unordered_map>
#include <algorithm>

#include "../../Util/Singleton.h"
#include "Object.h"

class Engine;

class Pool {
	friend class Engine;
private:

	template<typename T>
	class ObjectPool : public Singleton<ObjectPool<T>> {
	public:
		void Register(const std::shared_ptr<T>& obj) {
			m_Objects.push_back(obj);
		}

		void DeRegister(std::shared_ptr<T> obj) {
			auto iter = std::find(m_Objects.begin(), m_Objects.end(), obj);
			if (iter != m_Objects.end()) {
				m_Objects.erase(iter);
			}
		}

		template<class CallBack>
		void ForEach(CallBack func) {
			std::for_each(m_Objects.begin(), m_Objects.end(), func);
		}

		std::shared_ptr<T> Find(const int objId) {
			for (auto& ptr : m_Objects) {
				if (ptr->GetId() == objId) {
					return ptr;
				}
			}
			return std::shared_ptr<T>();
		}
		std::shared_ptr<T> Find(const std::string & objName) {
			for (auto& ptr : m_Objects) {
				if (ptr->Name == objName) {
					return ptr;
				}
			}
			return std::shared_ptr<T>();
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
				return iter->second->Get();
			}
			return std::shared_ptr<Object>();
		}

	private:
		class ObjWrapperBase {
		public:
			virtual std::shared_ptr<Object> Get() = 0;
		};

		template<typename T>
		class ObjectWrapper : public ObjWrapperBase {
		public:
			ObjectWrapper(const std::shared_ptr<T>& obj) : Ptr(obj) {
				ObjectPool<typename T::ManagedType>::GetInstance().Register(obj);
			}
			~ObjectWrapper() {
				ObjectPool<typename T::ManagedType>::GetInstance().DeRegister(Ptr);
			}
			virtual std::shared_ptr<Object> Get() override { return Ptr; }

		private:
			std::shared_ptr<T> Ptr;
		};

		std::unordered_map<int, std::shared_ptr<ObjWrapperBase>> m_Objects;
	};

public:

	template<typename T, typename ...Arg>
	static std::shared_ptr<T> CreateInstance(Arg&&... arg) {
		auto ptr = std::make_shared<T>(arg...);
		ObjectPool<Object>::GetInstance().Register<T>(ptr);
		return ptr;
	}

	static std::shared_ptr<Object> Find(const int objId) {
		return ObjectPool<Object>::GetInstance().Find(objId);
	}
	template<typename T>
	static std::shared_ptr<T> Find(const int objId) {
		return ObjectPool<T>::GetInstance().Find(objId);
	}
	template<typename T>
	static std::shared_ptr<T> Find(const std::string & objName) {
		return ObjectPool<T>::GetInstance().Find(objName);
	}

	static void Destroy(Object* obj);
};