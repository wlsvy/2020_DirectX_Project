#pragma once
#include "Component.h"
#include <DirectXMath.h>
#include <wrl/client.h>

struct ID3D11RenderTargetView;
struct ID3D11ShaderResourceView;
struct ID3D11Texture2D;

class LightBase abstract : public Component {
	COMPONENT_CONSTRUCTOR(LightBase, Component)
	MANAGED_OBJECT(LightBase)
public:
	void Awake() override;
	virtual void ProcessLight() {}
	ID3D11RenderTargetView *			GetShadowMapRenderTargetView() const		{ return m_ShadowMapRenderTargetView.Get(); }
	ID3D11RenderTargetView* const *		GetShadowMapRenderTargetViewAddr() const	{ return m_ShadowMapRenderTargetView.GetAddressOf(); }
	ID3D11ShaderResourceView *			GetShadowMapShaderResourceView() const		{ return m_ShadowMapShaderResourceView.Get(); }
	ID3D11ShaderResourceView* const *	GetShadowMapShaderResourceViewAddr() const	{ return m_ShadowMapShaderResourceView.GetAddressOf(); }
	
	void OnGui() override;

	static const float LIGHT_STRENGTH_MAX;

	DirectX::XMFLOAT3 Color = DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f);
	float Strength = 1.0f;
protected:

private:
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> m_ShadowMapRenderTargetView;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_ShadowMapShaderResourceView;
	Microsoft::WRL::ComPtr<ID3D11Texture2D> m_ShadowMapRenderTargetTexture;
};

class SpotLight : public LightBase {
	COMPONENT_CONSTRUCTOR(SpotLight, LightBase)
public:
	const DirectX::XMMATRIX & GetLightViewProjectMat() const;
	void ProcessLight() override;
	void OnGui() override;

	float Range = 10.0f;
	float SpotAngle = 1.0f;
	DirectX::XMFLOAT3 Attentuation = DirectX::XMFLOAT3(1.0f, 0.1f, 0.1f);

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
	DirectX::XMFLOAT3 Attentuation = DirectX::XMFLOAT3(1.0f, 0.1f, 0.1f);
};