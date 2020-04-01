#include "Object.h"

Object::Object()
{
	InitObjectID();
}

Object::Object(const std::string & name) : Name(name)
{
	InitObjectID();
}

void Object::InitObjectID()
{
	static int id = 0;
	m_Id = id++;
}
