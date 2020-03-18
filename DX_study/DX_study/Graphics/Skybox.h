#pragma once
#include <d3d11.h>
#include <wrl/client.h>
#include <string>
#include "Model.h"

class Skybox {
	friend class GraphicsManager;
public:

private:
	Skybox() {}
	bool Initialize(const std::string * filePath, ID3D11Device * _device, ID3D11DeviceContext * _deviceContext, Model * _model);
	bool Make_CubeMap();
	void Draw(const DirectX::XMMATRIX & wMatrix, const DirectX::XMMATRIX & viewProjectionMatrix);

	ID3D11Device * mDevice;
	ID3D11DeviceContext * mDeviceContext;

	Microsoft::WRL::ComPtr<ID3D11DepthStencilState> mSkyboxDepthStencilState = nullptr;
	Microsoft::WRL::ComPtr<ID3D11RasterizerState> mSkyboxRasterizerState = nullptr;

	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> mSkybox_RTV = nullptr;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> mSkybox_SRV[6] = { nullptr, };
	Microsoft::WRL::ComPtr<ID3D11Texture2D> mSkybox_Texture = nullptr;
	Microsoft::WRL::ComPtr<ID3D11Resource> mSkybox_Resource[6] = { nullptr, };
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> mSkybox_CubeMapSRV = nullptr;

	std::shared_ptr<Model> mSkyboxModel;

};