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
		friend class Engine;
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

	private:
		std::vector<std::shared_ptr<T>> m_Objects;
	};

public:
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
				ObjectPool<typename T::Base_Type>::GetInstance().Register(obj);
			}
			~ObjectWrapper() {
				ObjectPool<T>::GetInstance().DeRegister(Ptr);
			}
			virtual std::shared_ptr<Object> Get() override { return Ptr; }

		private:
			std::shared_ptr<T> Ptr;
		};

		std::unordered_map<int, std::shared_ptr<ObjWrapperBase>> m_Objects;
	};



	template<typename T>
	static std::shared_ptr<T> CreateInstance() {
		auto ptr = std::make_shared<T>();
		ObjectPool<Object>::GetInstance().Register<T>(ptr);
		return ptr;
	}

	static void Destroy(Object& obj);

};