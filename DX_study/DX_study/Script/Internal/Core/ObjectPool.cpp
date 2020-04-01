#include "ObjectPool.h"

void Destroy(Object & obj)
{
	ObjectPool<Object>::GetInstance().DeRegister(obj.GetId());
}
