#include "ObjectPool.h"

void Pool::Destroy(Object * obj)
{
	ObjectPool<Object>::GetInstance().DeRegister(obj->GetId());
}

void Pool::Destroy(std::shared_ptr<Object> obj)
{
	ObjectPool<Object>::GetInstance().DeRegister(obj->GetId());
}
