#pragma once

#include <SpriteBatch.h>
#include <SpriteFont.h>
#include <CommonStates.h>
#include <DirectXHelpers.h>
#include <Effects.h>
#include <PrimitiveBatch.h>
#include <VertexTypes.h>
#include "AdapterReader.h"

class DX11Resources {
public:
	bool Initialize(HWND hwnd, UINT width, UINT height);
	bool InitializeRenderTarget(UINT width, UINT height);
	bool InitializeDebugLayout(DirectX::XMMATRIX v, DirectX::XMMATRIX p);

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
	ID3D11RasterizerState*			GetRasterizerState() const							{ return m_RasterizerState.Get(); }
	ID3D11BlendState*				GetBlendState() const								{ return m_BlendState.Get();	}
	ID3D11SamplerState* const*		GetSamplerStateAddr() const							{ return m_SamplerState.GetAddressOf(); }
	ID3D11RenderTargetView*			GetRenderTargetView(int index) const				{ return m_RenderTargetViewArr[index].Get(); }
	ID3D11RenderTargetView* const*	GetRTVaddress(int index) const						{ return m_RenderTargetViewArr[index].GetAddressOf(); }
	ID3D11ShaderResourceView*		GetRenderTargetSrv(int index) const					{ return m_ShaderResourceViewArr[index].Get(); }
	ID3D11ShaderResourceView*const*	GetRenderTargetSrvAddress(int index) const			{ return m_ShaderResourceViewArr[index].GetAddressOf(); }
	ID3D11UnorderedAccessView*		GetRenderTargetUav(int index) const					{ return m_UnorderedAccessView[index].Get();}
	ID3D11UnorderedAccessView*const*GetRenderTargetUavAddr(int index) const				{ return m_UnorderedAccessView[index].GetAddressOf();}
	IDXGISwapChain*					GetSwapChain() const								{ return m_Swapchain.Get(); }

	DirectX::SpriteBatch*			GetSpriteBatch() const								{ return m_SpriteBatch.get(); }
	DirectX::SpriteFont*			GetSpriteFont() const								{ return m_SpriteFont.get(); }
	DirectX::CommonStates*			GetCommonState() const								{ return m_CommonState.get(); }
	DirectX::DebugEffect*			GetDebugEffect() const								{ return m_DebugEffect.get(); }
	DirectX::BasicEffect*			GetBasicEffect() const								{ return m_BasicEffect.get(); }
	DirectX::PrimitiveBatch<DirectX::VertexPositionColor>*	GetPrimitiveBatch() const	{ return m_PrimitiveBatch.get(); }
	ID3D11InputLayout*				GetDebugInputLayout() const							{ return m_DebugInputLayout.Get(); }

	static const int RenderTargetCount = 8;	//Position, Normal, Albedo, Mat, depth,	+ Result + blur * 2
	static const int DeferredRenderChannelCount = 5; 

protected:
	Microsoft::WRL::ComPtr<ID3D11Device> m_Device;
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> m_DeviceContext; 
	Microsoft::WRL::ComPtr<IDXGISwapChain> m_Swapchain;

	Microsoft::WRL::ComPtr<ID3D11RasterizerState> m_RasterizerState;
	Microsoft::WRL::ComPtr<ID3D11BlendState> m_BlendState;

	//SamplerState
	Microsoft::WRL::ComPtr<ID3D11SamplerState> m_SamplerState;
	Microsoft::WRL::ComPtr<ID3D11SamplerState> m_SamplerPointClamp;
	Microsoft::WRL::ComPtr<ID3D11SamplerState> m_SamplerLinearClamp;
	Microsoft::WRL::ComPtr<ID3D11SamplerState> m_SamplerLinearWrap;
	Microsoft::WRL::ComPtr<ID3D11SamplerState> m_SamplerAnisotropicWrap;


	//Depth/Stencil
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView> m_MainDepthStencilView;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_MainDepthStencilSRV;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView> m_SubDepthStencilView;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_SubDepthStencilSRV;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilState> m_DepthStencilState;

	//RenderTargets
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> m_MainRenderTargetView;
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> m_RenderTargetViewArr[RenderTargetCount];
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_ShaderResourceViewArr[RenderTargetCount];
	Microsoft::WRL::ComPtr<ID3D11Texture2D> m_RenderTargetTextureArr[RenderTargetCount];
	Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView> m_UnorderedAccessView[RenderTargetCount];

	ID3D11RenderTargetView * const m_NullRtv[DX11Resources::RenderTargetCount] = { NULL, };
	ID3D11ShaderResourceView * const m_NullSrv[DX11Resources::RenderTargetCount] = { NULL, };
	ID3D11UnorderedAccessView * const m_NullUav[DX11Resources::RenderTargetCount] = { NULL, };

	//DirectX Tool Kit - DirectXTK
	std::unique_ptr<DirectX::SpriteBatch> m_SpriteBatch;
	std::unique_ptr<DirectX::SpriteFont> m_SpriteFont;
	std::unique_ptr<DirectX::CommonStates> m_CommonState;
	std::unique_ptr<DirectX::BasicEffect> m_BasicEffect;
	std::unique_ptr<DirectX::DebugEffect> m_DebugEffect;
	std::unique_ptr<DirectX::PrimitiveBatch<DirectX::VertexPositionColor>> m_PrimitiveBatch;
	Microsoft::WRL::ComPtr<ID3D11InputLayout> m_DebugInputLayout;

	
};