#include "Light_ver2.h"

DirectionalLight::DirectionalLight(const COMPONENT_INIT_DESC & desc) : Light_ver2(desc) { std::strcpy(mComponentName, "Directional Light"); }

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

SpotLight::SpotLight(const COMPONENT_INIT_DESC & desc) : Light_ver2(desc) { std::strcpy(mComponentName, "Spot Light"); }

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

PointLight::PointLight(const COMPONENT_INIT_DESC & desc) : Light_ver2(desc) { std::strcpy(mComponentName, "Point Light"); }

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

Light_ver2::Light_ver2(const COMPONENT_INIT_DESC & desc) : Component(desc), transform(desc.mTransform) {}
