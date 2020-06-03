#include "Object.h"
#include "ObjectPool.h"

Object::Object()
{
	InitObject();
}

Object::Object(const std::string & name) : Name(name)
{
	InitObject();
}

Object::Object(const Object & obj) : Name(obj.Name)
{
	InitObject();
}

Object::Object(Object && obj) : 
	Name(std::move(obj.Name)), 
	m_Id(std::move(obj.m_Id))
{
}

Object & Object::operator=(const Object & obj)
{
	Name = obj.Name;
	return *this;
}

void Object::InitObject()
{
	static unsigned int s_Id = 0;
	m_Id = s_Id++;
}
