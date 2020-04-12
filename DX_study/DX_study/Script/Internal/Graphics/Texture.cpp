#include "Texture.h"
#include "../../Util/ErrorLogger.h"
#include <WICTextureLoader.h>
#include <DDSTextureLoader.h>

#include "../Core/InternalHelper.h"
#include "../../Util/StringHelper.h"

Texture::Texture(const Color & color, aiTextureType type)
{
	this->Initialize1x1ColorTexture(color, type);
}

Texture::Texture(const Color * colorData, UINT width, UINT height, aiTextureType type)
{
	this->InitializeColorTexture(colorData, width, height, type);
}

Texture::Texture(const std::string & filePath, aiTextureType type) : Object(StringHelper::GetFileNameFromPath(filePath))
{
	
	this->type = type;
	if (StringHelper::GetFileExtension(filePath) == ".dds")
	{
		HRESULT hr = DirectX::CreateDDSTextureFromFile(Core::GetDevice(), StringHelper::StringToWide(filePath).c_str(), texture.GetAddressOf(), this->textureView.GetAddressOf());
		if (FAILED(hr))
		{
			this->Initialize1x1ColorTexture(Colors::UnloadedTextureColor, type);
		}
		return;
	}
	else
	{
		HRESULT hr = DirectX::CreateWICTextureFromFile(Core::GetDevice(), StringHelper::StringToWide(filePath).c_str(), texture.GetAddressOf(), this->textureView.GetAddressOf());
		if (FAILED(hr))
		{
			this->Initialize1x1ColorTexture(Colors::UnloadedTextureColor, type);
		}
		return;
	}
}

Texture::Texture(const uint8_t * pData, size_t size, aiTextureType type) 
{
	this->type = type;
	HRESULT hr = DirectX::CreateWICTextureFromMemory(Core::GetDevice(), pData, size, this->texture.GetAddressOf(), this->textureView.GetAddressOf());
	ThrowIfFailed(hr, "Failed to create Texture from memory.");
}

void Texture::Initialize1x1ColorTexture(const Color & colorData, aiTextureType type)
{
	InitializeColorTexture(&colorData, 1, 1, type);
}

void Texture::InitializeColorTexture(const Color * colorData, UINT width, UINT height, aiTextureType type)
{
	this->type = type;
	CD3D11_TEXTURE2D_DESC textureDesc(DXGI_FORMAT_R8G8B8A8_UNORM, width, height);
	ID3D11Texture2D * p2DTexture = nullptr;
	D3D11_SUBRESOURCE_DATA initialData{};
	initialData.pSysMem = colorData;
	initialData.SysMemPitch = width * sizeof(Color);
	HRESULT hr = Core::GetDevice()->CreateTexture2D(&textureDesc, &initialData, &p2DTexture);
	ThrowIfFailed(hr, "Failed to initialize texture from color data.");
	texture = static_cast<ID3D11Texture2D*>(p2DTexture);
	CD3D11_SHADER_RESOURCE_VIEW_DESC srvDesc(D3D11_SRV_DIMENSION_TEXTURE2D, textureDesc.Format);
	hr = Core::GetDevice()->CreateShaderResourceView(texture.Get(), &srvDesc, textureView.GetAddressOf());
	ThrowIfFailed(hr, "Failed to create shader resource view from texture generated from color data.");
}