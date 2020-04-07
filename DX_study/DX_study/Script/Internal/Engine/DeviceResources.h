#pragma once

#include <SpriteBatch.h>
#include <SpriteFont.h>
#include "AdapterReader.h"
#include "../../Util/Singleton.h"

class DeviceResources {
public:
	bool Initialize(HWND hwnd, int width, int height);
	bool InitializeRenderTarget(int width, int height);

	ID3D11Device*					GetDevice()	const						{ return device.Get(); }
	ID3D11DeviceContext*			GetDeviceContext()	const				{ return deviceContext.Get(); }
	ID3D11RenderTargetView*			GetBaseRenderTargetView() const			{ return renderTargetView.Get(); }
	ID3D11RenderTargetView* const*	GetBaseRenderTargetViewAddress() const	{ return renderTargetView.GetAddressOf(); }
	ID3D11DepthStencilView*			GetBaseDepthStencilView() const			{ return depthStencilView.Get(); }
	ID3D11DepthStencilState*		GetBaseDepthStencilState() const		{ return depthStencilState.Get(); }
	ID3D11RasterizerState*			GetRasterizerState() const				{ return rasterizerState.Get(); }
	ID3D11BlendState*				GetBlendState() const					{ return blendState.Get();	}
	ID3D11SamplerState* const*		GetSamplerStateAddr() const				{ return samplerState.GetAddressOf(); }
	DirectX::SpriteBatch*			GetSpriteBatch() const					{ return spriteBatch.get(); }
	DirectX::SpriteFont*			GetSpriteFont() const					{ return spriteFont.get(); }
	ID3D11RenderTargetView*			GetAuxRenderTargetView() const			{ return auxiliaryRenderTargetView.Get(); }
	ID3D11RenderTargetView* const*	GetAuxRenderTargetViewAddress() const	{ return auxiliaryRenderTargetView.GetAddressOf(); }
	ID3D11ShaderResourceView*		GetAuxRenderTargetSrv() const			{ return auxiliaryShaderResourceView.Get(); }
	IDXGISwapChain*					GetSwapChain() const					{ return swapchain.Get(); }

private:
	Microsoft::WRL::ComPtr<ID3D11Device> device; //����̽� �������̽� : ��� ���� ���˰� �ڿ� �Ҵ翡 ����
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> deviceContext; //����̽� ���ؽ�Ʈ �������̽� : ���� ����� �����ϰ� �ڿ��� �׷��� ���������ο� ���� Gpu�� ������ ������ ��ɵ��� �����ϴµ� ���δ�.
	Microsoft::WRL::ComPtr<IDXGISwapChain> swapchain; //����Ʈ ���� �� ���� �ٲ�ġ��
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> renderTargetView;

	Microsoft::WRL::ComPtr<ID3D11DepthStencilView> depthStencilView;
	Microsoft::WRL::ComPtr<ID3D11Texture2D> depthStencilBuffer;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilState> depthStencilState;

	Microsoft::WRL::ComPtr<ID3D11RasterizerState> rasterizerState;
	Microsoft::WRL::ComPtr<ID3D11BlendState> blendState;
	Microsoft::WRL::ComPtr<ID3D11SamplerState> samplerState;

	std::unique_ptr<DirectX::SpriteBatch> spriteBatch;
	std::unique_ptr<DirectX::SpriteFont> spriteFont;

	//����Ÿ�� �߰�
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> auxiliaryRenderTargetView;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> auxiliaryShaderResourceView;
	Microsoft::WRL::ComPtr<ID3D11Texture2D> auxiliaryRenderTargetTexture;
};