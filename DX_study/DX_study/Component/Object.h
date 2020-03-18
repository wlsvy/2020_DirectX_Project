#ifndef OBJECT_H
#define OBJECT_H
#include <string>
#include "../UI/imGui/imgui.h"

class Object {
public:
	virtual void Destroy() {}
	//void Initialize();
	virtual void OnGui() {}
};

#endif