#pragma once

#include <SpriteBatch.h>
#include <SpriteFont.h>
#include <CommonStates.h>
#include <DirectXHelpers.h>
#include <Effects.h>
#include <PrimitiveBatch.h>
#include <VertexTypes.h>
#include "AdapterReader.h"

struct RenderTargetTypes {
	enum {
		Position = 0,
		Normal,
		Albedo,
		Material,
		Depth,
		Composition,
		BlurIn,
		BlurOut,
		SSAO,
		Max
	};
};

class DX11Resources {
public:
	bool Initialize(HWND hwnd, UINT width, UINT height);
	bool InitializeDebugLayout(DirectX::XMMATRIX v, DirectX::XMMATRIX p);

	void CreateRasterizerState(
		ID3D11RasterizerState** addr,
		D3D11_FILL_MODE fillMode = D3D11_FILL_SOLID,
		D3D11_CULL_MODE cullMode = D3D11_CULL_BACK);
	void CreateDepthStencilState(ID3D11DepthStencilState** addr);
	void CreateSamplerState(
		ID3D11SamplerState ** addr,
		D3D11_FILTER filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR,
		D3D11_TEXTURE_ADDRESS_MODE addressMode = D3D11_TEXTURE_ADDRESS_WRAP);
	void CreateBlenderState(
		ID3D11BlendState ** addr,
		bool blendEnable = true, 
		D3D11_BLEND SrcBlend = D3D11_BLEND::D3D11_BLEND_SRC_ALPHA,
		D3D11_BLEND DestBlend = D3D11_BLEND::D3D11_BLEND_INV_SRC_ALPHA);
	void CreateRenderTarget(
		ID3D11RenderTargetView** rtv,
		ID3D11ShaderResourceView** srv,
		ID3D11UnorderedAccessView** uav,
		UINT widht, 
		UINT height,
		DXGI_FORMAT format = DXGI_FORMAT_R32G32B32A32_FLOAT
	);

	void SetVertexBuffer(ID3D11Buffer *const* vertexBuffer, const UINT * stridePtr);
	void SetIndexBuffer(ID3D11Buffer * indexBuffer);

	void SetVSConstantBuffer(UINT startSlot, ID3D11Buffer *const* buffer);
	void SetVSInputLayout(ID3D11InputLayout * inputLayout);
	void SetVertexShader(ID3D11VertexShader* shader);

	void SetPsSampler(UINT startSlot, ID3D11SamplerState ** sampler);
	void SetPSConstantBuffer(UINT startSlot, ID3D11Buffer *const* buffer);
	void SetPixelShader(ID3D11PixelShader* shader);
	void SetPSShaderResources(UINT startSlot, UINT range, ID3D11ShaderResourceView*const* srv);

	void SetGSConstantBuffer(UINT startSlot, ID3D11Buffer *const* buffer);
	void SetGeometryShader(ID3D11GeometryShader* shader);

	void SetComputeShader(ID3D11ComputeShader* shader);
	void SetCSConstantBuffer(UINT startSlot, ID3D11Buffer *const* buffer);
	void SetCSShaderResources(UINT startSlot, UINT range, ID3D11ShaderResourceView*const* srv);
	void SetCSUavResources(UINT startSlot, UINT range, ID3D11UnorderedAccessView*const* uav);

	void SetBlendState(ID3D11BlendState* blendState, const float* factor);
	void SetDepthStencilState(ID3D11DepthStencilState * state);
	void SetRasterizerState(ID3D11RasterizerState* rasterizer);
	void SetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY topology);
	void SetRenderTarget(
		UINT numViews, 
		ID3D11RenderTargetView* const* renderTargetView, 
		ID3D11DepthStencilView* depthStencilView);

	void DrawIndexed(const UINT indexCount);
	void DispatchComputeShader(const UINT X, const UINT Y, const UINT Z = 1);

	ID3D11Device*					GetDevice()	const									{ return m_Device.Get(); }
	ID3D11DeviceContext*			GetDeviceContext()	const							{ return m_DeviceContext.Get(); }
	ID3D11RenderTargetView*			GetBaseRenderTargetView() const						{ return m_MainRenderTargetView.Get(); }
	ID3D11RenderTargetView* const*	GetBaseRTVaddress() const							{ return m_MainRenderTargetView.GetAddressOf(); }
	ID3D11DepthStencilView*			GetBaseDepthStencilView() const						{ return m_MainDepthStencilView.Get(); }
	ID3D11ShaderResourceView*		GetBaseDepthStencilSrv() const						{ return m_MainDepthStencilSRV.Get(); }
	ID3D11ShaderResourceView*const*	GetBaseDepthStencilSrvAddress() const				{ return m_MainDepthStencilSRV.GetAddressOf(); }
	ID3D11DepthStencilView*			GetSubDepthStencilView() const						{ return m_SubDepthStencilView.Get(); }
	ID3D11ShaderResourceView*		GetSubDepthStencilSrv() const						{ return m_SubDepthStencilSRV.Get(); }
	ID3D11ShaderResourceView*const*	GetSubDepthStencilSrvAddress() const				{ return m_SubDepthStencilSRV.GetAddressOf(); }
	ID3D11DepthStencilState*		GetBaseDepthStencilState() const					{ return m_DepthStencilState.Get(); }
	ID3D11RasterizerState*			GetRasterizerState() const							{ return m_RasterizerCullBack.Get(); }
	ID3D11BlendState*				GetBlendState() const								{ return m_BlendStateAlpha.Get();	}
	ID3D11RenderTargetView*			GetRenderTargetView(int index) const				{ return m_RenderTargetViewArr[index].Get(); }
	ID3D11RenderTargetView* const*	GetRTVaddress(int index) const						{ return m_RenderTargetViewArr[index].GetAddressOf(); }
	ID3D11ShaderResourceView*		GetRenderTargetSrv(int index) const					{ return m_RenderTargetSrvs[index].Get(); }
	ID3D11ShaderResourceView*const*	GetRenderTargetSrvAddress(int index) const			{ return m_RenderTargetSrvs[index].GetAddressOf(); }
	ID3D11UnorderedAccessView*		GetRenderTargetUav(int index) const					{ return m_RenderTargetUavs[index].Get();}
	ID3D11UnorderedAccessView*const*GetRenderTargetUavAddr(int index) const				{ return m_RenderTargetUavs[index].GetAddressOf();}
	IDXGISwapChain*					GetSwapChain() const								{ return m_Swapchain.Get(); }

	DirectX::SpriteBatch*			GetSpriteBatch() const								{ return m_SpriteBatch.get(); }
	DirectX::SpriteFont*			GetSpriteFont() const								{ return m_SpriteFont.get(); }
	DirectX::CommonStates*			GetCommonState() const								{ return m_CommonState.get(); }
	DirectX::DebugEffect*			GetDebugEffect() const								{ return m_DebugEffect.get(); }
	DirectX::BasicEffect*			GetBasicEffect() const								{ return m_BasicEffect.get(); }
	DirectX::PrimitiveBatch<DirectX::VertexPositionColor>*	GetPrimitiveBatch() const	{ return m_PrimitiveBatch.get(); }
	ID3D11InputLayout*				GetDebugInputLayout() const							{ return m_DebugInputLayout.Get(); }

	static const UINT MAX_RENDER_TARGET_BINDING_COUNT = 5; 
	static const UINT MAX_SHADER_RESOURCE_VIEW_BINDING_COUNT = 12;
	static const UINT MAX_UNORDERED_ACCESS_VIEW_BINDING_COUNT = 2;

protected:
	Microsoft::WRL::ComPtr<ID3D11Device> m_Device;
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> m_DeviceContext; 
	Microsoft::WRL::ComPtr<IDXGISwapChain> m_Swapchain;

	//RasterizerState
	Microsoft::WRL::ComPtr<ID3D11RasterizerState> m_RasterizerCullBack;
	Microsoft::WRL::ComPtr<ID3D11RasterizerState> m_RasterizerCullFront;
	Microsoft::WRL::ComPtr<ID3D11RasterizerState> m_RasterizerCullNone;

	//blendState
	Microsoft::WRL::ComPtr<ID3D11BlendState> m_BlendStateOpaque;
	Microsoft::WRL::ComPtr<ID3D11BlendState> m_BlendStateAlpha;
	Microsoft::WRL::ComPtr<ID3D11BlendState> m_BlendStateAdditive;

	//SamplerState
	Microsoft::WRL::ComPtr<ID3D11SamplerState> m_SamplerPointClamp;
	Microsoft::WRL::ComPtr<ID3D11SamplerState> m_SamplerLinearClamp;
	Microsoft::WRL::ComPtr<ID3D11SamplerState> m_SamplerLinearWrap;
	Microsoft::WRL::ComPtr<ID3D11SamplerState> m_SamplerLinearMirror;
	Microsoft::WRL::ComPtr<ID3D11SamplerState> m_SamplerAnisotropicWrap;

	//Depth/Stencil
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView> m_MainDepthStencilView;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_MainDepthStencilSRV;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView> m_SubDepthStencilView;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_SubDepthStencilSRV;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilState> m_DepthStencilState;

	//RenderTargets
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> m_MainRenderTargetView;
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> m_RenderTargetViewArr[RenderTargetTypes::Max];
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_RenderTargetSrvs[RenderTargetTypes::Max];
	Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView> m_RenderTargetUavs[RenderTargetTypes::Max];
	Microsoft::WRL::ComPtr<ID3D11Texture2D> m_RenderTargetTextureArr[RenderTargetTypes::Max];

	ID3D11RenderTargetView * const m_NullRtv[MAX_RENDER_TARGET_BINDING_COUNT] = { NULL, };
	ID3D11ShaderResourceView * const m_NullSrv[MAX_SHADER_RESOURCE_VIEW_BINDING_COUNT] = { NULL, };
	ID3D11UnorderedAccessView * const m_NullUav[MAX_UNORDERED_ACCESS_VIEW_BINDING_COUNT] = { NULL, };

	//DirectX Tool Kit - DirectXTK
	std::unique_ptr<DirectX::SpriteBatch> m_SpriteBatch;
	std::unique_ptr<DirectX::SpriteFont> m_SpriteFont;
	std::unique_ptr<DirectX::CommonStates> m_CommonState;
	std::unique_ptr<DirectX::BasicEffect> m_BasicEffect;
	std::unique_ptr<DirectX::DebugEffect> m_DebugEffect;
	std::unique_ptr<DirectX::PrimitiveBatch<DirectX::VertexPositionColor>> m_PrimitiveBatch;
	Microsoft::WRL::ComPtr<ID3D11InputLayout> m_DebugInputLayout;

	
};