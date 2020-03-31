#include "Light.h"

bool Light::Initialize()
{
	if (!model.Initialize("Data/Objects/light.fbx"))
		return false;

	this->SetPosition(0.0f, 0.0f, 0.0f);
	this->SetRotation(0.0f, 0.0f, 0.0f);
	this->UpdateMatrix();
	return true;
}
