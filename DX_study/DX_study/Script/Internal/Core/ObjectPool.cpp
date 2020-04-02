#include "ObjectPool.h"

void Pool::Destroy(Object & obj)
{
	ObjectPool<Object>::GetInstance().DeRegister(obj.GetId());
}
