#include "Skybox.h"
#include <WICTextureLoader.h>
#include <assimp/material.h>
#include <DDSTextureLoader.h>
#include <DirectXMath.h>

#include "../Core/InternalHelper.h"
#include "../Core/ObjectPool.h"
#include "../../Util/ErrorLogger.h"
#include "Shaders.h"
#include "Model.h"


bool Skybox::Initialize(const std::string * filePath)
{
	//mSkyboxModel = std::shared_ptr<Model>(_model);

	HRESULT hr;
	try {
		for (int Index = 0; Index < 6; Index++) {
			std::wstring filename = StringHelper::StringToWide(filePath[Index]);
			hr = DirectX::CreateWICTextureFromFile(Core::GetDevice(), filename.c_str(), mSkybox_Resource[Index].GetAddressOf(), this->mSkybox_SRV[Index].GetAddressOf());
			ThrowIfFailed(hr, "Skybox Texture initialize failed.");
		}
	}
	catch (CustomException & exception) {
		StringHelper::ErrorLog(exception);
		return false;
	}

	if (!InitializeCubeMap())
		return false;
	if (!InitializeIrMap())
		return false;

	m_Model = Core::Find<Model>("sphere");

	m_Vshader = Core::Find<VertexShader>("SkyboxModel");
	m_Pshader = Core::Find<PixelShader>("Skybox");

	return true;

}

bool Skybox::InitializeCubeMap()
{
	//큐브맵 만드는 함수 출처 : https://stackoverflow.com/questions/19364012/d3d11-creating-a-cube-map-from-6-images
	try {
		//Each element in the texture array has the same format/dimensions.
		HRESULT hr;
		D3D11_TEXTURE2D_DESC texElementDesc;
		((ID3D11Texture2D*)mSkybox_Resource[0].Get())->GetDesc(&texElementDesc);

		D3D11_TEXTURE2D_DESC texArrayDesc;
		texArrayDesc.Width = texElementDesc.Width;
		texArrayDesc.Height = texElementDesc.Height;
		texArrayDesc.MipLevels = 0;
		texArrayDesc.ArraySize = 6;
		texArrayDesc.Format = texElementDesc.Format;
		texArrayDesc.SampleDesc.Count = 1;
		texArrayDesc.SampleDesc.Quality = 0;
		texArrayDesc.Usage = D3D11_USAGE_DEFAULT;
		texArrayDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS | D3D11_BIND_RENDER_TARGET;
		texArrayDesc.CPUAccessFlags = 0;
		texArrayDesc.MiscFlags = D3D11_RESOURCE_MISC_TEXTURECUBE | D3D11_RESOURCE_MISC_GENERATE_MIPS;;

		ID3D11Texture2D* texArray = 0;
		ThrowIfFailed(
			Core::GetDevice()->CreateTexture2D(&texArrayDesc, 0, &texArray), 
			"Failed to create CubeMap Texture.");

		texArray->GetDesc(&texArrayDesc);
		m_MaxMipLevel = texArrayDesc.MipLevels;

		D3D11_BOX sourceRegion;
		for (UINT x = 0; x < texArrayDesc.ArraySize; x++)
		{
			sourceRegion.left = 0;
			sourceRegion.top = 0;
			sourceRegion.right = texArrayDesc.Width;
			sourceRegion.bottom = texArrayDesc.Height;
			sourceRegion.front = 0;
			sourceRegion.back = 1;

			if (sourceRegion.bottom == 0 || sourceRegion.right == 0) {
				break;
			}

			UINT mipLevel = 0;
			Core::GetDeviceContext()->CopySubresourceRegion(texArray, D3D11CalcSubresource(mipLevel, x, m_MaxMipLevel), 0, 0, 0, mSkybox_Resource[x].Get(), mipLevel, &sourceRegion);
		}

		//Create a resource view to the texture array.
		D3D11_SHADER_RESOURCE_VIEW_DESC viewDesc;
		viewDesc.Format = texArrayDesc.Format;
		viewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBE;
		viewDesc.TextureCube.MostDetailedMip = 0;
		viewDesc.TextureCube.MipLevels = -1;

		ThrowIfFailed(
			Core::GetDevice()->CreateShaderResourceView(texArray, &viewDesc, &mSkybox_CubeMapSRV),
			"Failed to create CubeMap Texture SRV.");
		Core::GetDeviceContext()->GenerateMips(mSkybox_CubeMapSRV.Get());

	}
	catch (CustomException & exception) {
		StringHelper::ErrorLog(exception);
		return false;
	}

	return true;
}

bool Skybox::InitializeIrMap()
{
	//큐브맵 만드는 함수 출처 : https://stackoverflow.com/questions/19364012/d3d11-creating-a-cube-map-from-6-images
	try {
		D3D11_TEXTURE2D_DESC texElementDesc;
		((ID3D11Texture2D*)mSkybox_Resource[0].Get())->GetDesc(&texElementDesc);

		D3D11_TEXTURE2D_DESC texArrayDesc;
		texArrayDesc.Width = texElementDesc.Width/8;
		texArrayDesc.Height = texElementDesc.Height/8;
		texArrayDesc.MipLevels = 0;
		texArrayDesc.ArraySize = 6;
		texArrayDesc.Format = texElementDesc.Format;
		texArrayDesc.SampleDesc.Count = 1;
		texArrayDesc.SampleDesc.Quality = 0;
		texArrayDesc.Usage = D3D11_USAGE_DEFAULT;
		texArrayDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS | D3D11_BIND_RENDER_TARGET;
		texArrayDesc.CPUAccessFlags = 0;
		texArrayDesc.MiscFlags = D3D11_RESOURCE_MISC_TEXTURECUBE | D3D11_RESOURCE_MISC_GENERATE_MIPS;

		Microsoft::WRL::ComPtr<ID3D11Texture2D> texArray;
		ThrowIfFailed(Core::GetDevice()->CreateTexture2D(&texArrayDesc, 0, texArray.GetAddressOf()), "Failed to create CubeMap Texture.");

		D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
		srvDesc.Format = texArrayDesc.Format;
		srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBE;
		srvDesc.TextureCube.MostDetailedMip = 0;
		srvDesc.TextureCube.MipLevels = -1;

		ThrowIfFailed(Core::GetDevice()->CreateShaderResourceView(texArray.Get(), &srvDesc, m_IrMapSrv.GetAddressOf()), "Failed to create CubeMap srv.");

		D3D11_UNORDERED_ACCESS_VIEW_DESC uavDesc = {};
		uavDesc.Format = texArrayDesc.Format;
		uavDesc.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE2DARRAY;
		uavDesc.Texture2DArray.FirstArraySlice = 0;
		uavDesc.Texture2DArray.ArraySize = texArrayDesc.ArraySize;
		uavDesc.Texture2D.MipSlice = 0;
		ThrowIfFailed(Core::GetDevice()->CreateUnorderedAccessView(texArray.Get(), &uavDesc, m_IrMapUav.GetAddressOf()), "Failed to create CubeMap srv.");


		{
			auto irMapComputeShader = Core::Find<ComputeShader>("IrMap");

			Core::GetDeviceContext()->CSSetShaderResources(0, 1, mSkybox_CubeMapSRV.GetAddressOf());
			Core::GetDeviceContext()->CSSetUnorderedAccessViews(0, 1, m_IrMapUav.GetAddressOf(), nullptr);
			Core::GetDeviceContext()->CSSetShader(irMapComputeShader->GetShader(), nullptr, 0);
			Core::GetDeviceContext()->Dispatch(texArrayDesc.Width / 8, texArrayDesc.Height / 8, 6);

			ID3D11ShaderResourceView *  nullSrv = NULL;
			ID3D11UnorderedAccessView * nullUav = NULL;
			Core::GetDeviceContext()->CSSetShaderResources(0, 1, &nullSrv);
			Core::GetDeviceContext()->CSSetUnorderedAccessViews(0, 1, &nullUav, nullptr);
		}

		Core::GetDeviceContext()->GenerateMips(m_IrMapSrv.Get());
	}
	catch (CustomException & exception) {
		StringHelper::ErrorLog(exception);
		return false;
	}

	return true;
}