#pragma once
#include "Component.h"
#include "GameObject_v2.h"
#include "../Timer.h"

class Behaviour : public Component {
public:
	
protected:
	Behaviour(GameObject_v2 & obj);

	template<class T>
	T* AddComponent();
	template<class T>
	T* GetComponent();
	
};

#include "Animator.h"

template<class T>
inline T * Behaviour::AddComponent()
{
	return GameObject.AddComponent<T>();
}

template<class T>
inline T * Behaviour::GetComponent()
{
	return GameObject.GetComponent<T>();
}