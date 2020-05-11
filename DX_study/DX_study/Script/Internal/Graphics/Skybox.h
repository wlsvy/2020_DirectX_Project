#pragma once
#include <d3d11.h>
#include <wrl/client.h>
#include <string>

namespace DirectX {
	struct XMMATRIX;
}
class PixelShader;
class VertexShader;
class Model;

class Skybox {
public:
	Skybox() {}
	~Skybox() {}
	bool Initialize(const std::string * filePath);

	ID3D11ShaderResourceView * GetCubeMapSrv() const { return mSkybox_CubeMapSRV.Get(); }
	ID3D11ShaderResourceView * GetIrMapSrv() const { return m_IrMapSrv.Get(); }
	ID3D11ShaderResourceView *const* GetCubeMapView() const { return mSkybox_CubeMapSRV.GetAddressOf(); }
	ID3D11ShaderResourceView *const* GetIrMapView() const { return m_IrMapSrv.GetAddressOf(); }
	std::shared_ptr<Model> GetModel() { return m_Model; }
	PixelShader* GetPixelShader() { return m_Pshader.get(); }
	VertexShader* GetVertexShader() { return m_Vshader.get(); }

private:
	bool InitializeCubeMap();
	bool InitializeIrMap();

	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> mSkybox_RTV = nullptr;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> mSkybox_SRV[6] = { nullptr, };
	Microsoft::WRL::ComPtr<ID3D11Texture2D> mSkybox_Texture = nullptr;
	Microsoft::WRL::ComPtr<ID3D11Resource> mSkybox_Resource[6] = { nullptr, };
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> mSkybox_CubeMapSRV = nullptr;

	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_IrMapSrv;
	Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView> m_IrMapUav;

	std::shared_ptr<PixelShader> m_Pshader;
	std::shared_ptr<VertexShader> m_Vshader;
	std::shared_ptr<Model> m_Model;
};