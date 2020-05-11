#pragma once
#include <vector>
#include <memory>
#include <unordered_map>
#include <algorithm>

#include "../../Util/Singleton.h"
#include "Object.h"

namespace Core {
	template<typename T>
	class Pool : public Singleton<Pool<T>> {
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

		const std::vector<std::shared_ptr<T>> & GetItems() const {
			return m_Objects;
		}

	protected:
		std::vector<std::shared_ptr<T>> m_Objects;
	};

	template<>
	class Pool<Object> : public Singleton<Pool<Object>> {
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

		void Clear() { m_Objects.clear(); }

	private:
		struct ObjWrapperBase {
			virtual std::shared_ptr<Object> Get() = 0;
		};

		template<typename T>
		struct ObjectWrapper : ObjWrapperBase {
			ObjectWrapper(const std::shared_ptr<T>& obj) : Ptr(obj) {
				Pool<typename T::ManagedType>::GetInstance().Register(Ptr);
			}
			~ObjectWrapper() {
				Pool<typename T::ManagedType>::GetInstance().DeRegister(Ptr);
			}
			virtual std::shared_ptr<Object> Get() override { return Ptr; }

		private:
			std::shared_ptr<T> Ptr;
		};

		std::unordered_map<int, std::unique_ptr<ObjWrapperBase>> m_Objects;
	};

	template<typename T, typename ...Arg>
	std::shared_ptr<T> CreateInstance(Arg&&... arg) {
		auto ptr = std::make_shared<T>(arg...);
		Pool<Object>::GetInstance().Register<typename T::ManagedType>(ptr);
		return ptr;
	}

	template<typename T>
	std::shared_ptr<T> Find(const int objId) {
		auto ptr = Pool<typename T::ManagedType>::GetInstance().Find(objId);
		return std::dynamic_pointer_cast<T>(ptr);
	}
	template<typename T>
	std::shared_ptr<T> Find(const std::string & objName) {
		auto ptr = Pool<typename T::ManagedType>::GetInstance().Find(objName);
		return std::dynamic_pointer_cast<T>(ptr);
	}

	std::shared_ptr<Object> Find(const int objId);
	void Destroy(Object* obj);
	void Destroy(std::shared_ptr<Object> obj);
};