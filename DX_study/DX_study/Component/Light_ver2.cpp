#include "Light_ver2.h"
#include "GameObject_v2.h"

DirectionalLight::DirectionalLight(GameObject_v2 & obj) : Light_ver2(obj) 
{ 
	std::strcpy(Name, "Directional Light"); 
}

LIGHT_INFO_DESC DirectionalLight::GetInfoDesc()
{
	LIGHT_INFO_DESC lightDesc = {
		&lightType,
		&transform->GetPositionFloat3(),
		&transform->GetRotationFloat3(),
		&lightColor,
		nullptr,
		nullptr,
		&lightStrength,
		nullptr
	};

	return lightDesc;
}

SpotLight::SpotLight(GameObject_v2 & obj) : Light_ver2(obj) 
{
	std::strcpy(Name, "Spot Light");
}

LIGHT_INFO_DESC  SpotLight::GetInfoDesc()
{
	LIGHT_INFO_DESC lightDesc = {
		&lightType,
		&transform->GetPositionFloat3(),
		&transform->GetRotationFloat3(),
		&lightColor,
		&Attentuation,
		&Range,
		&lightStrength,
		&SpotAngle
	};

	return lightDesc;
}

PointLight::PointLight(GameObject_v2 & obj) : Light_ver2(obj) 
{ 
	std::strcpy(Name, "Point Light"); 
}

LIGHT_INFO_DESC  PointLight::GetInfoDesc()
{
	LIGHT_INFO_DESC lightDesc = {
		&lightType,
		&transform->GetPositionFloat3(),
		&transform->GetRotationFloat3(),
		&lightColor,
		&Attentuation,
		&Range,
		&lightStrength,
		nullptr
	};

	return lightDesc;
}

Light_ver2::Light_ver2(GameObject_v2 & obj) : Component(obj), transform(obj.transform) {}
