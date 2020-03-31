#include "Object.h"

Object::Object()
{
	static int id = 0;
	Id = id++;
}
