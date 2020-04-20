#pragma once
#include "Component.h"
#include <DirectXMath.h>

class LightBase abstract : public Component {
	COMPONENT_CONSTRUCTOR(LightBase, Component)
public:
	void Awake() override;

	DirectX::XMFLOAT3 Color = DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f);
	DirectX::XMFLOAT3 Attentuation = DirectX::XMFLOAT3(1.0f, 0.1f, 0.1f);
	float Strength = 1.0f;

protected:

};

class SpotLight : public LightBase {
	COMPONENT_CONSTRUCTOR(SpotLight, LightBase)
public:
	const DirectX::XMMATRIX & GetLightViewProjectMat() const;
	void SetProjectionMatrix();
	void OnGui() override;

	float Range = 10.0f;
	float SpotAngle = 1.0f;

private:
	DirectX::XMMATRIX m_ProjectionMatrix;
};

class DirectionalLight : public LightBase {
	COMPONENT_CONSTRUCTOR(DirectionalLight, LightBase)
public:

};

class PointLight : public LightBase {
	COMPONENT_CONSTRUCTOR(PointLight, LightBase)
public:
	float Range = 10.0f;
};