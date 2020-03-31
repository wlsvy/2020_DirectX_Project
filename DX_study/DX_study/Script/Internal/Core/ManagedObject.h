#pragma once
#pragma once
#include <vector>
#include <memory>

template<typename T>
class ManagedObject {
public:
	void Update() {
		for (auto& o : m_Objects) {
			//m_Objects->Update();
		}
	}

	void Register(const std::shared_ptr<T>& obj) { 
		m_Objects.push_back(obj); 
	}

	void DeRegister(const std::shared_ptr<T>& obj) {
		auto iter = m_Objects.find(m_Objects.begin().m_Objects.end(), obj);
		if (iter != m_Objects.end()) {
			m_Objects.erase(iter);
		}
	}
private:
	std::vector<std::shared_ptr<T>> m_Objects;
};