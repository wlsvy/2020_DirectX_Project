#pragma once
#include "Component.h"
#include "Renderable.h"
#include "../Util/Math.h"
#include <Queue>
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
	virtual bool CullRenderable(const DirectX::BoundingBox &) { return false; }

	void PushToRenderQueue(const Renderable & renderable)							{ m_ShaderMapRenderQueue.push(renderable); }
	ID3D11RenderTargetView *			GetShadowMapRenderTargetView() const		{ return m_ShadowMapRenderTargetView.Get(); }
	ID3D11RenderTargetView* const *		GetShadowMapRenderTargetViewAddr() const	{ return m_ShadowMapRenderTargetView.GetAddressOf(); }
	ID3D11ShaderResourceView *			GetShadowMapShaderResourceView() const		{ return m_ShadowMapShaderResourceView.Get(); }
	ID3D11ShaderResourceView* const *	GetShadowMapShaderResourceViewAddr() const	{ return m_ShadowMapShaderResourceView.GetAddressOf(); }

	void OnGui() override;

	static const float LIGHT_STRENGTH_MAX;

	DirectX::XMFLOAT3 Color = DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f);
	float Strength = 1.0f;
protected:
	std::queue<Renderable> m_ShaderMapRenderQueue;

private:
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> m_ShadowMapRenderTargetView;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_ShadowMapShaderResourceView;
	Microsoft::WRL::ComPtr<ID3D11Texture2D> m_ShadowMapRenderTargetTexture;
};

class SpotLight : public LightBase {
	COMPONENT_CONSTRUCTOR(SpotLight, LightBase)
public:
	DirectX::XMMATRIX GetLightViewProjectMat() const;
	bool CullRenderable(const DirectX::BoundingBox &) override;
	void ProcessLight() override;
	void OnGui() override;

	void SetRange(float range);
	void SetSpotAngle(float angle);

	DirectX::XMFLOAT3 Attentuation = DirectX::XMFLOAT3(1.0f, 0.1f, 0.1f);

private:
	void SetProjectionMatrix();

	float m_Range = 10.0f;
	float m_SpotAngle = 1.0f;

	DirectX::XMMATRIX m_ProjectionMatrix;
	DirectX::BoundingFrustum m_Frustum;
};

class DirectionalLight : public LightBase {
	COMPONENT_CONSTRUCTOR(DirectionalLight, LightBase)
public:
	bool CullRenderable(const DirectX::BoundingBox &) override { return true; }
};

class PointLight : public LightBase {
	COMPONENT_CONSTRUCTOR(PointLight, LightBase)
public:
	bool CullRenderable(const DirectX::BoundingBox &) override;

	float Range = 10.0f;
	DirectX::XMFLOAT3 Attentuation = DirectX::XMFLOAT3(1.0f, 0.1f, 0.1f);
};