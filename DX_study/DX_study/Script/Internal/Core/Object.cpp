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

Object::~Object()
{
}

void Object::InitObject()
{
	static int s_Id = 0;
	m_Id = s_Id++;
}
