#include "Light.h"

bool Light::Initialize(ConstantBuffer<CB_VS_vertexshader>& cb_vs_vertexshader)
{
	if (!model.Initialize("Data/Objects/light.fbx", cb_vs_vertexshader))
		return false;

	this->SetPosition(0.0f, 0.0f, 0.0f);
	this->SetRotation(0.0f, 0.0f, 0.0f);
	this->UpdateMatrix();
	return true;
}
