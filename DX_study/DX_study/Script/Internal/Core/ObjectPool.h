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
		friend class Pool<Object>;
	public:
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

		~Pool() {
			int a = 0;
		}

	private:
		void Register(const std::shared_ptr<T>& obj) {
			m_Objects.push_back(obj);
		}

		void DeRegister(std::shared_ptr<T> obj) {
			auto iter = std::find(m_Objects.begin(), m_Objects.end(), obj);
			if (iter != m_Objects.end()) {
				m_Objects.erase(iter);
			}
		}

		std::vector<std::shared_ptr<T>> m_Objects;
	};

	template<>
	class Pool<Object> : public Singleton<Pool<Object>> {
	public:
		template<typename T, typename ...Arg>
		friend std::shared_ptr<T> CreateInstance(Arg&&... arg);
		friend void Destroy(const Object* obj);
		friend void Destroy(const std::shared_ptr<Object> & obj);

		void Clear() { m_Objects.clear(); }

	private:
		struct ElementBase {
			virtual std::shared_ptr<Object> Get() = 0;
		};

		template<typename T>
		struct Element : ElementBase {
			Element(const std::shared_ptr<T>& obj) : Ptr(obj) {
				Pool<typename T::ManagedType>::GetInstance().Register(Ptr);
			}
			~Element() {
				Pool<typename T::ManagedType>::GetInstance().DeRegister(Ptr);
			}
			virtual std::shared_ptr<Object> Get() override { return Ptr; }

		private:
			std::shared_ptr<T> Ptr;
		};

		template<typename T>
		void Register(const std::shared_ptr<T>& obj) {
			m_Objects.insert(make_pair(obj->GetId(), std::make_unique<Element<T>>(obj)));
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

		std::unordered_map<int, std::unique_ptr<ElementBase>> m_Objects;
	};

	template<typename T, typename ...Arg>
	std::shared_ptr<T> CreateInstance(Arg&&... arg) {
		auto ptr = std::make_shared<T>(std::forward<Arg>(arg)...);
		Pool<Object>::GetInstance().Register<typename T::ManagedType>(ptr);
		return ptr;
	}

	template<typename T>
	std::shared_ptr<T> Find(const unsigned int objId) {
		auto ptr = Pool<typename T::ManagedType>::GetInstance().Find(objId);
		return std::dynamic_pointer_cast<T>(ptr);
	}
	template<typename T>
	std::shared_ptr<T> Find(const std::string & objName) {
		auto ptr = Pool<typename T::ManagedType>::GetInstance().Find(objName);
		return std::dynamic_pointer_cast<T>(ptr);
	}

	inline void Destroy(const Object* obj)
	{
		if (obj != nullptr) {
			Pool<Object>::GetInstance().DeRegister(obj->GetId());
		}
	}
	inline void Destroy(const std::shared_ptr<Object> & obj)
	{
		if (obj) {
			Pool<Object>::GetInstance().DeRegister(obj->GetId());
		}
	}
};