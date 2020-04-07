#pragma once
#include <d3d11.h>
#include <wrl/client.h>
#include <string>

namespace DirectX {
	struct XMMATRIX;
}

class Skybox {
public:
	Skybox() {}
	~Skybox() {}
	bool Initialize(const std::string * filePath);
	bool Make_CubeMap();
	void Draw(const DirectX::XMMATRIX & wMatrix, const DirectX::XMMATRIX & viewProjectionMatrix);

private:
	Microsoft::WRL::ComPtr<ID3D11DepthStencilState> mSkyboxDepthStencilState = nullptr;
	Microsoft::WRL::ComPtr<ID3D11RasterizerState> mSkyboxRasterizerState = nullptr;

	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> mSkybox_RTV = nullptr;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> mSkybox_SRV[6] = { nullptr, };
	Microsoft::WRL::ComPtr<ID3D11Texture2D> mSkybox_Texture = nullptr;
	Microsoft::WRL::ComPtr<ID3D11Resource> mSkybox_Resource[6] = { nullptr, };
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> mSkybox_CubeMapSRV = nullptr;
};