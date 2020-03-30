#pragma once

#include <SpriteBatch.h>
#include <SpriteFont.h>
#include "Graphics/AdapterReader.h"
#include "../Util/Singleton.h"

class DeviceResources : public Singleton<DeviceResources> {
public:
	bool Initialize(HWND hwnd, int width, int height);

	ID3D11Device*				GetDevice()	const					{ return device.Get(); }
	ID3D11DeviceContext*		GetDeviceContext()	const			{ return deviceContext.Get(); }
	ID3D11RenderTargetView*		GetBaseRenderTargetView() const		{ return renderTargetView.Get(); }
	ID3D11DepthStencilView*		GetBaseDepthStencilView() const		{ return depthStencilView.Get(); }
	ID3D11DepthStencilState*	GetBaseDepthStencilState() const	{ return depthStencilState.Get(); }
	ID3D11RasterizerState*		GetRasterizerState() const			{ return rasterizerState.Get(); }
	ID3D11BlendState*			GetBlendState() const				{ return blendState.Get();	}
	ID3D11SamplerState* const*	GetSamplerStateAddr() const			{ return samplerState.GetAddressOf(); }
	DirectX::SpriteBatch*		GetSpriteBatch() const				{ return spriteBatch.get(); }
	DirectX::SpriteFont*		GetSpriteFont() const				{ return spriteFont.get(); }

	void PresentSwapChain()		{ swapchain->Present(1, NULL); }

private:
	Microsoft::WRL::ComPtr<ID3D11Device> device; //디바이스 인터페이스 : 기능 지원 점검과 자원 할당에 쓰임
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> deviceContext; //디바이스 컨텍스트 인터페이스 : 렌더 대상을 설정하고 자원을 그래픽 파이프라인에 묶고 Gpu가 수행할 렌더링 명령들을 지시하는데 쓰인다.
	Microsoft::WRL::ComPtr<IDXGISwapChain> swapchain; //프론트 버퍼 백 버퍼 바꿔치기
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> renderTargetView;

	Microsoft::WRL::ComPtr<ID3D11DepthStencilView> depthStencilView;
	Microsoft::WRL::ComPtr<ID3D11Texture2D> depthStencilBuffer;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilState> depthStencilState;

	Microsoft::WRL::ComPtr<ID3D11RasterizerState> rasterizerState;
	Microsoft::WRL::ComPtr<ID3D11BlendState> blendState;
	Microsoft::WRL::ComPtr<ID3D11SamplerState> samplerState;

	std::unique_ptr<DirectX::SpriteBatch> spriteBatch;
	std::unique_ptr<DirectX::SpriteFont> spriteFont;
};