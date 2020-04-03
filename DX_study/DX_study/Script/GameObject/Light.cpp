#include "Light.h"

bool Light::Initialize()
{
	if (!model.Initialize("Data/Objects/light.fbx"))
		return false;

	return true;
}
