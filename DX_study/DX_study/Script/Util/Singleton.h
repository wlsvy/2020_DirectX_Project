#pragma once
#include <memory>
#include "Uncopyable.h"

template<typename T>
class Singleton : public Uncopyable
{
public:
	static T& GetInstance() 
	{ 
		if (s_Instance == nullptr) {
			s_Instance = new T;
		}
		return *s_Instance; 
	}

	static std::unique_ptr<T> CreateUnique() 
	{
		auto unique = std::make_unique<T>();
		Singleton<T>::s_Instance = unique.get();
		return unique;
	}

protected:
	Singleton() {}
	virtual ~Singleton() 
	{ 
		s_Instance = nullptr; 
	}

private:
	static T* s_Instance;
};

template<typename T>
T* Singleton<T>::s_Instance = nullptr;