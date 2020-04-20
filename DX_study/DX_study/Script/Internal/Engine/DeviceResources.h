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
	void SetDebugLayout(DirectX::XMMATRIX v, DirectX::XMMATRIX p);

	ID3D11Device*					GetDevice()	const								{ return device.Get(); }
	ID3D11DeviceContext*			GetDeviceContext()	const						{ return deviceContext.Get(); }
	ID3D11RenderTargetView*			GetBaseRenderTargetView() const					{ return mainRenderTargetView.Get(); }
	ID3D11RenderTargetView* const*	GetBaseRTVaddress() const						{ return mainRenderTargetView.GetAddressOf(); }
	ID3D11DepthStencilView*			GetBaseDepthStencilView() const					{ return depthStencilView.Get(); }
	ID3D11DepthStencilState*		GetBaseDepthStencilState() const				{ return depthStencilState.Get(); }
	ID3D11RasterizerState*			GetRasterizerState() const						{ return rasterizerState.Get(); }
	ID3D11BlendState*				GetBlendState() const							{ return blendState.Get();	}
	ID3D11SamplerState* const*		GetSamplerStateAddr() const						{ return samplerState.GetAddressOf(); }
	ID3D11RenderTargetView*			GetRenderTargetView(int index) const			{ return renderTargetViewArr[index].Get(); }
	ID3D11RenderTargetView* const*	GetRTVaddress(int index) const					{ return renderTargetViewArr[index].GetAddressOf(); }
	ID3D11ShaderResourceView*		GetRenderTargetSrv(int index) const				{ return shaderResourceViewArr[index].Get(); }
	ID3D11ShaderResourceView*const*	GetRenderTargetSrvAddress(int index) const		{ return shaderResourceViewArr[index].GetAddressOf(); }
	IDXGISwapChain*					GetSwapChain() const							{ return swapchain.Get(); }

	DirectX::SpriteBatch*			GetSpriteBatch() const							{ return spriteBatch.get(); }
	DirectX::SpriteFont*			GetSpriteFont() const							{ return spriteFont.get(); }
	DirectX::CommonStates*			GetCommonState() const							{ return commonState.get(); }
	DirectX::DebugEffect*			GetDebugEffect() const							{ return debugEffect.get(); }
	DirectX::BasicEffect*			GetBasicEffect() const							{ return basicEffect.get(); }
	DirectX::PrimitiveBatch<DirectX::VertexPositionColor>*	GetPrimitiveBatch() const	{ return primitiveBatch.get(); }
	ID3D11InputLayout*				GetDebugInputLayout() const						{ return debugInputLayout.Get(); }

	static const int RenderTargetCount = 4;	//Position, Normal, Color	+ Result
	static const int DeferredRenderChannelCount = 3; 

private:
	Microsoft::WRL::ComPtr<ID3D11Device> device; //디바이스 인터페이스 : 기능 지원 점검과 자원 할당에 쓰임
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> deviceContext; //디바이스 컨텍스트 인터페이스 : 렌더 대상을 설정하고 자원을 그래픽 파이프라인에 묶고 Gpu가 수행할 렌더링 명령들을 지시하는데 쓰인다.
	Microsoft::WRL::ComPtr<IDXGISwapChain> swapchain; //프론트 버퍼 백 버퍼 바꿔치기
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> mainRenderTargetView;

	Microsoft::WRL::ComPtr<ID3D11DepthStencilView> depthStencilView;
	Microsoft::WRL::ComPtr<ID3D11Texture2D> depthStencilBuffer;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilState> depthStencilState;

	Microsoft::WRL::ComPtr<ID3D11RasterizerState> rasterizerState;
	Microsoft::WRL::ComPtr<ID3D11BlendState> blendState;
	Microsoft::WRL::ComPtr<ID3D11SamplerState> samplerState;

	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> renderTargetViewArr[RenderTargetCount];
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> shaderResourceViewArr[RenderTargetCount];
	Microsoft::WRL::ComPtr<ID3D11Texture2D> renderTargetTextureArr[RenderTargetCount];

#pragma region DirectX Tool Kit
	std::unique_ptr<DirectX::SpriteBatch> spriteBatch;
	std::unique_ptr<DirectX::SpriteFont> spriteFont;
	std::unique_ptr<DirectX::CommonStates> commonState;
	std::unique_ptr<DirectX::BasicEffect> basicEffect;
	std::unique_ptr<DirectX::DebugEffect> debugEffect;
	std::unique_ptr<DirectX::PrimitiveBatch<DirectX::VertexPositionColor>> primitiveBatch;
	Microsoft::WRL::ComPtr<ID3D11InputLayout> debugInputLayout;
#pragma endregion

	
};