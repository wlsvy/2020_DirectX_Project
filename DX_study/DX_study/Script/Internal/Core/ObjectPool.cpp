#include "ObjectPool.h"

std::shared_ptr<Object> Core::Find(const int objId) {
	return Pool<Object>::GetInstance().Find(objId);
}

void Core::Destroy(Object * obj)
{
	Pool<Object>::GetInstance().DeRegister(obj->GetId());
}

void Core::Destroy(std::shared_ptr<Object> obj)
{
	Pool<Object>::GetInstance().DeRegister(obj->GetId());
}
