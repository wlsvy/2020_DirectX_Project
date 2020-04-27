#pragma once

#include <SpriteBatch.h>
#include <SpriteFont.h>
#include <CommonStates.h>
#include <DirectXHelpers.h>
#include <Effects.h>
#include <PrimitiveBatch.h>
#include <VertexTypes.h>
#include "AdapterReader.h"

class DeviceResources {
public:
	bool Initialize(HWND hwnd, int width, int height);
	bool InitializeRenderTarget(int width, int height);
	bool InitializeDebugLayout(DirectX::XMMATRIX v, DirectX::XMMATRIX p);

	ID3D11Device*					GetDevice()	const									{ return device.Get(); }
	ID3D11DeviceContext*			GetDeviceContext()	const							{ return deviceContext.Get(); }
	ID3D11RenderTargetView*			GetBaseRenderTargetView() const						{ return mainRenderTargetView.Get(); }
	ID3D11RenderTargetView* const*	GetBaseRTVaddress() const							{ return mainRenderTargetView.GetAddressOf(); }
	ID3D11DepthStencilView*			GetBaseDepthStencilView() const						{ return mainDepthStencilView.Get(); }
	ID3D11DepthStencilView*			GetSubDepthStencilView() const						{ return subDepthStencilView.Get(); }
	ID3D11DepthStencilState*		GetBaseDepthStencilState() const					{ return depthStencilState.Get(); }
	ID3D11RasterizerState*			GetRasterizerState() const							{ return rasterizerState.Get(); }
	ID3D11BlendState*				GetBlendState() const								{ return blendState.Get();	}
	ID3D11SamplerState* const*		GetSamplerStateAddr() const							{ return samplerState.GetAddressOf(); }
	ID3D11RenderTargetView*			GetRenderTargetView(int index) const				{ return renderTargetViewArr[index].Get(); }
	ID3D11RenderTargetView* const*	GetRTVaddress(int index) const						{ return renderTargetViewArr[index].GetAddressOf(); }
	ID3D11ShaderResourceView*		GetRenderTargetSrv(int index) const					{ return shaderResourceViewArr[index].Get(); }
	ID3D11ShaderResourceView*const*	GetRenderTargetSrvAddress(int index) const			{ return shaderResourceViewArr[index].GetAddressOf(); }
	IDXGISwapChain*					GetSwapChain() const								{ return swapchain.Get(); }

	DirectX::SpriteBatch*			GetSpriteBatch() const								{ return spriteBatch.get(); }
	DirectX::SpriteFont*			GetSpriteFont() const								{ return spriteFont.get(); }
	DirectX::CommonStates*			GetCommonState() const								{ return commonState.get(); }
	DirectX::DebugEffect*			GetDebugEffect() const								{ return debugEffect.get(); }
	DirectX::BasicEffect*			GetBasicEffect() const								{ return basicEffect.get(); }
	DirectX::PrimitiveBatch<DirectX::VertexPositionColor>*	GetPrimitiveBatch() const	{ return primitiveBatch.get(); }
	ID3D11InputLayout*				GetDebugInputLayout() const							{ return debugInputLayout.Get(); }

	static const int RenderTargetCount = 7;	//Position, Normal, Color, light	+ Result + blur * 2
	static const int DeferredRenderChannelCount = 4; 

private:
	Microsoft::WRL::ComPtr<ID3D11Device> device;
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> deviceContext; 
	Microsoft::WRL::ComPtr<IDXGISwapChain> swapchain;

	Microsoft::WRL::ComPtr<ID3D11RasterizerState> rasterizerState;
	Microsoft::WRL::ComPtr<ID3D11BlendState> blendState;
	Microsoft::WRL::ComPtr<ID3D11SamplerState> samplerState;
	
	//Depth/Stencil
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView> mainDepthStencilView;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView> subDepthStencilView;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilState> depthStencilState;

	//RenderTargets
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> mainRenderTargetView;
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> renderTargetViewArr[RenderTargetCount];
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> shaderResourceViewArr[RenderTargetCount];
	Microsoft::WRL::ComPtr<ID3D11Texture2D> renderTargetTextureArr[RenderTargetCount];
	Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView> unorderedAccessView[RenderTargetCount];

	//DirectX Tool Kit - DirectX XTK
	std::unique_ptr<DirectX::SpriteBatch> spriteBatch;
	std::unique_ptr<DirectX::SpriteFont> spriteFont;
	std::unique_ptr<DirectX::CommonStates> commonState;
	std::unique_ptr<DirectX::BasicEffect> basicEffect;
	std::unique_ptr<DirectX::DebugEffect> debugEffect;
	std::unique_ptr<DirectX::PrimitiveBatch<DirectX::VertexPositionColor>> primitiveBatch;
	Microsoft::WRL::ComPtr<ID3D11InputLayout> debugInputLayout;

	
};