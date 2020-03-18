#include "Skybox.h"
#include <WICTextureLoader.h>
#include "..\\ErrorLogger.h"
#include <string>
#include <assimp/material.h>
#include <DDSTextureLoader.h>
#include "BaseGeometry.h"


bool Skybox::Initialize(const std::string * filePath, ID3D11Device * _device, ID3D11DeviceContext * _deviceContext, Model * _model)
{
	mDeviceContext = _deviceContext;
	mDevice = _device;

	Study_DX::Sphere testSphere(30, 30);
	mSkyboxModel = std::shared_ptr<Model>(_model);

	HRESULT hr;
	try {
		for (int Index = 0; Index < 6; Index++) {
			std::wstring filename = StringHelper::StringToWide(filePath[Index]);
			hr = DirectX::CreateWICTextureFromFile(mDevice, filename.c_str(), mSkybox_Resource[Index].GetAddressOf(), this->mSkybox_SRV[Index].GetAddressOf());
			COM_ERROR_IF_FAILED(hr, "Skybox Texture initialize failed.");
		}

		CD3D11_DEPTH_STENCIL_DESC depthstencildesc(D3D11_DEFAULT);
		depthstencildesc.DepthFunc = D3D11_COMPARISON_FUNC::D3D11_COMPARISON_LESS_EQUAL;
		hr = this->mDevice->CreateDepthStencilState(&depthstencildesc, this->mSkyboxDepthStencilState.GetAddressOf());
		COM_ERROR_IF_FAILED(hr, "Failed to create skybox depth stencil state.");

		CD3D11_RASTERIZER_DESC rasterizerDesc(D3D11_DEFAULT);
		rasterizerDesc.CullMode = D3D11_CULL_NONE;
		hr = this->mDevice->CreateRasterizerState(&rasterizerDesc, this->mSkyboxRasterizerState.GetAddressOf());
		COM_ERROR_IF_FAILED(hr, "Failed to create skybox rasterizer state.");
	}
	catch (COMException & exception) {
		ErrorLogger::Log(exception);
		return false;
	}

	if (!Make_CubeMap())
		return false;

	return true;

}

bool Skybox::Make_CubeMap()
{
	//큐브맵 만드는 함수 출처 : https://stackoverflow.com/questions/19364012/d3d11-creating-a-cube-map-from-6-images
	try {
		// Each element in the texture array has the same format/dimensions.
		HRESULT hr;
		D3D11_TEXTURE2D_DESC texElementDesc;
		((ID3D11Texture2D*)mSkybox_Resource[0].Get())->GetDesc(&texElementDesc);

		D3D11_TEXTURE2D_DESC texArrayDesc;
		texArrayDesc.Width = texElementDesc.Width;
		texArrayDesc.Height = texElementDesc.Height;
		texArrayDesc.MipLevels = texElementDesc.MipLevels;
		texArrayDesc.ArraySize = 6;
		texArrayDesc.Format = texElementDesc.Format;
		texArrayDesc.SampleDesc.Count = 1;
		texArrayDesc.SampleDesc.Quality = 0;
		texArrayDesc.Usage = D3D11_USAGE_DEFAULT;
		texArrayDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
		texArrayDesc.CPUAccessFlags = 0;
		texArrayDesc.MiscFlags = D3D11_RESOURCE_MISC_TEXTURECUBE;

		ID3D11Texture2D* texArray = 0;
		hr = mDevice->CreateTexture2D(&texArrayDesc, 0, &texArray);
		COM_ERROR_IF_FAILED(hr, "Failed to create CubeMap Texture.");

		// Copy individual texture elements into texture array.
		ID3D11DeviceContext* pd3dContext;
		mDevice->GetImmediateContext(&pd3dContext);
		D3D11_BOX sourceRegion;

		//Here i copy the mip map levels of the textures
		for (UINT x = 0; x < 6; x++)
		{
			for (UINT mipLevel = 0; mipLevel < texArrayDesc.MipLevels; mipLevel++)
			{
				sourceRegion.left = 0;
				sourceRegion.right = (texArrayDesc.Width >> mipLevel);
				sourceRegion.top = 0;
				sourceRegion.bottom = (texArrayDesc.Height >> mipLevel);
				sourceRegion.front = 0;
				sourceRegion.back = 1;

				//test for overflow
				if (sourceRegion.bottom == 0 || sourceRegion.right == 0)
					break;

				mDeviceContext->CopySubresourceRegion(texArray, D3D11CalcSubresource(mipLevel, x, texArrayDesc.MipLevels), 0, 0, 0, mSkybox_Resource[x].Get(), mipLevel, &sourceRegion);
			}
		}

		// Create a resource view to the texture array.
		D3D11_SHADER_RESOURCE_VIEW_DESC viewDesc;
		viewDesc.Format = texArrayDesc.Format;
		viewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBE;
		viewDesc.TextureCube.MostDetailedMip = 0;
		viewDesc.TextureCube.MipLevels = texArrayDesc.MipLevels;

		hr = mDevice->CreateShaderResourceView(texArray, &viewDesc, &mSkybox_CubeMapSRV);
		COM_ERROR_IF_FAILED(hr, "Failed to create CubeMap Texture SRV.");
	}
	catch (COMException & exception) {
		ErrorLogger::Log(exception);
		return false;
	}

	return true;
}

void Skybox::Draw(const DirectX::XMMATRIX & wMatrix, const DirectX::XMMATRIX & viewProjectionMatrix)
{
	mDeviceContext->RSSetState(this->mSkyboxRasterizerState.Get());//래스터라이저 설정 적용
	mDeviceContext->OMSetDepthStencilState(this->mSkyboxDepthStencilState.Get(), 0);
	mDeviceContext->PSSetShaderResources(1, 1, mSkybox_CubeMapSRV.GetAddressOf());

	mSkyboxModel->Draw(wMatrix, viewProjectionMatrix);
}
