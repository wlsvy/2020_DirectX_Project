#pragma once
#include <string>

#include "../UI/imGui/imgui.h"

class Object {
public:
	virtual void Destroy() {}
	virtual void OnGui() {}
};
