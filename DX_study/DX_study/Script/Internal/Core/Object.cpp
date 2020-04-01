#include "Object.h"
#include "ManagedObject.h"

Object::Object()
{
	InitObject();
}

Object::Object(const std::string & name) : Name(name)
{
	InitObject();
}

Object::~Object()
{
}

void Destroy(Object& obj)
{
	ObjectPool<Object>::GetInstance().DeRegister(obj.GetId());
}

void Object::InitObject()
{
	static int id = 0;
	m_Id = id++;
}
