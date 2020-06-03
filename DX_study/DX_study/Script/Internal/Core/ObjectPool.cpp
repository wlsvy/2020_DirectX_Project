#include "ObjectPool.h"

void Core::Destroy(Object * obj)
{
	Pool<Object>::GetInstance().DeRegister(obj->GetId());
}

void Core::Destroy(std::shared_ptr<Object> obj)
{
	Pool<Object>::GetInstance().DeRegister(obj->GetId());
}
