#pragma once
#include <memory>
#include "Uncopyable.h"

template<typename T>
class Singleton : public Uncopyable
{
public:
	static T& GetInstance();

protected:
	Singleton() {}
	virtual ~Singleton() { s_Instance = nullptr; }

private:
	static T* s_Instance;
};

template<typename T>
T* Singleton<T>::s_Instance = nullptr;

template<typename T>
inline T& Singleton<T>::GetInstance()
{
	if (Singleton<T>::s_Instance == nullptr) {
		Singleton<T>::s_Instance = new T();
	}
	return (*Singleton<T>::s_Instance);
}