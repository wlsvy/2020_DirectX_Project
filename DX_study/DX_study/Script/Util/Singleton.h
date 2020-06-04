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
			atexit([]()
			{
				if (s_Instance) {
					delete s_Instance;
					s_Instance = nullptr;
				}
			});
		}
		
		return *s_Instance; 
	}

protected:
	Singleton() {}
	virtual ~Singleton() {}

private:
	static T* s_Instance;
};

template<typename T>
T* Singleton<T>::s_Instance = nullptr;