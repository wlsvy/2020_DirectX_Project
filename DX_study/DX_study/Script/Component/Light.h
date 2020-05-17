#pragma once
#include "Component.h"
#include "RenderInfo.h"
#include "../Util/Math.h"
#include <Queue>
#include <wrl/client.h>

struct ID3D11RenderTargetView;
struct ID3D11ShaderResourceView;
struct ID3D11Texture2D;
class Graphics;

class LightBase abstract : public Component {
	COMPONENT_CONSTRUCTOR(LightBase, Component)
	MANAGED_OBJECT(LightBase)
public:
	void Awake() override;
	virtual void ProcessLight() {}
	virtual bool CullRenderable(const DirectX::BoundingBox &) { return true; }
	virtual bool IsShadowEnable() { return false; }

	ID3D11RenderTargetView *			GetShadowMapRenderTargetView() const		{ return m_ShadowMapRenderTargetView.Get(); }
	ID3D11RenderTargetView* const *		GetShadowMapRenderTargetViewAddr() const	{ return m_ShadowMapRenderTargetView.GetAddressOf(); }
	ID3D11ShaderResourceView *			GetShadowMapShaderResourceView() const		{ return m_ShadowMapShaderResourceView.Get(); }
	ID3D11ShaderResourceView* const *	GetShadowMapShaderResourceViewAddr() const	{ return m_ShadowMapShaderResourceView.GetAddressOf(); }

	void OnGui(const char* option = "-1") override;

	static const float LIGHT_STRENGTH_MAX;

	DirectX::XMFLOAT3 Color = DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f);
	float Strength = 40.0f;

public:
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> m_ShadowMapRenderTargetView;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_ShadowMapShaderResourceView;
	
};

class SpotLight : public LightBase {
	COMPONENT_CONSTRUCTOR(SpotLight, LightBase)
public:
	void Awake() override { LightBase::Awake(); SetProjectionMatrix(); }

	const DirectX::XMMATRIX & GetProjectionMatrix() const { return m_ProjectionMatrix; }

	DirectX::XMMATRIX GetLightViewProjectMat() const;
	bool CullRenderable(const DirectX::BoundingBox &) override;
	bool IsShadowEnable() override { return m_IsShadowEnable; }
	void ProcessLight() override;
	void OnGui(const char* option = "-1") override;

	void SetRange(float range);
	void SetSpotAngle(float angle);

	float GetRange() const { return m_Range; }
	float GetSpotAngle() const { return m_SpotAngle; }

	DirectX::XMFLOAT3 Attentuation = DirectX::XMFLOAT3(0.1f, 0.1f, 0.01f);

private:
	void SetProjectionMatrix();

	bool m_IsShadowEnable = true;
	float m_Range = 50.0f;
	float m_SpotAngle = 130.0f;

	DirectX::XMMATRIX m_ProjectionMatrix;
	DirectX::BoundingFrustum m_Frustum;
};

class DirectionalLight : public LightBase {
	COMPONENT_CONSTRUCTOR(DirectionalLight, LightBase)
public:
	bool IsShadowEnable() override { return m_IsShadowEnable; }
	bool CullRenderable(const DirectX::BoundingBox &) override { return false; }

private:

	bool m_IsShadowEnable = true;
};