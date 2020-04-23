#pragma once
#include <memory>
#include "Uncopyable.h"

template<typename T>
class Singleton : public Uncopyable
{
public:
	static T& GetInstance() 
	{ 
		static T* s_Instance = new T;
		return *s_Instance; 
	}

protected:
	Singleton() {}
	virtual ~Singleton() {}
};
