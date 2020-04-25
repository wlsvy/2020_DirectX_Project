#pragma once
#include "../../Component/Light.h"
#include "../../Util/Singleton.h"

struct ID3D11RenderTargetView;
struct ID3D11ShaderResourceView;
struct ID3D11DepthStencilView;

class ShadowMap : public Singleton<ShadowMap> {
public:
	bool Initialize();
	void DrawShadowMap();
	void DrawReult();

	ID3D11RenderTargetView *			GetShadowMapRenderTargetView() const { return m_ShadowMapRenderTargetView.Get(); }
	ID3D11RenderTargetView* const *		GetShadowMapRenderTargetViewAddr() const { return m_ShadowMapRenderTargetView.GetAddressOf(); }
	ID3D11ShaderResourceView *			GetShadowMapShaderResourceView() const { return m_ShadowMapShaderResourceView.Get(); }
	ID3D11ShaderResourceView* const *	GetShadowMapShaderResourceViewAddr() const { return m_ShadowMapShaderResourceView.GetAddressOf(); }

	void OnGui() ;

	std::shared_ptr<SpotLight> ShadowCaster;

public:
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> m_ShadowMapRenderTargetView;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_ShadowMapShaderResourceView;
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> m_ResultRenderTargetView;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_ResultShaderResourceView;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView> m_DepthStencilView;

	DirectX::XMMATRIX vpMat;
};