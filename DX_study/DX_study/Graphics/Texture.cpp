#include "Texture.h"
#include "..\\ErrorLogger.h"
#include <WICTextureLoader.h>
#include <DDSTextureLoader.h>

#include "../Engine/ModuleResource.h"

Texture::Texture(const Color & color, aiTextureType type)
{
	this->Initialize1x1ColorTexture(color, type);
}

Texture::Texture(const Color * colorData, UINT width, UINT height, aiTextureType type)
{
	this->InitializeColorTexture(colorData, width, height, type);
}

Texture::Texture(const std::string & filePath, aiTextureType type)
{
	this->type = type;
	if (StringHelper::GetFileExtension(filePath) == ".dds")
	{
		HRESULT hr = DirectX::CreateDDSTextureFromFile(&Module::GetDevice(), StringHelper::StringToWide(filePath).c_str(), texture.GetAddressOf(), this->textureView.GetAddressOf());
		if (FAILED(hr))
		{
			this->Initialize1x1ColorTexture(CustomColors::UnloadedTextureColor, type);
		}
		return;
	}
	else
	{
		HRESULT hr = DirectX::CreateWICTextureFromFile(&Module::GetDevice(), StringHelper::StringToWide(filePath).c_str(), texture.GetAddressOf(), this->textureView.GetAddressOf());
		if (FAILED(hr))
		{
			this->Initialize1x1ColorTexture(CustomColors::UnloadedTextureColor, type);
		}
		return;
	}
}

Texture::Texture(const uint8_t * pData, size_t size, aiTextureType type)
{
	this->type = type;
	HRESULT hr = DirectX::CreateWICTextureFromMemory(&Module::GetDevice(), pData, size, this->texture.GetAddressOf(), this->textureView.GetAddressOf());
	COM_ERROR_IF_FAILED(hr, "Failed to create Texture from memory.");
}

Texture::Texture(const Texture & _rhs)
{
	Name = _rhs.Name;
	texture = _rhs.texture;
	textureView = _rhs.textureView;
	type = _rhs.type;
}

Texture::Texture(Texture && _rhs)
{
	Name = _rhs.Name;
	texture = _rhs.texture;
	textureView = _rhs.textureView;
	type = _rhs.type;
}

aiTextureType Texture::GetType()
{
	return this->type;
}

ID3D11ShaderResourceView * Texture::GetTextureResourceView()
{
	return this->textureView.Get();
}

ID3D11ShaderResourceView ** Texture::GetTextureResourceViewAddress()
{
	return this->textureView.GetAddressOf();
}

void Texture::Initialize1x1ColorTexture(const Color & colorData, aiTextureType type)
{
	InitializeColorTexture(&colorData, 1, 1, type);
}

void Texture::InitializeColorTexture(const Color * colorData, UINT width, UINT height, aiTextureType type)
{
	this->type = type;

	CD3D11_TEXTURE2D_DESC textureDesc(DXGI_FORMAT_R8G8B8A8_UNORM, width, height); //텍스쳐생성
	ID3D11Texture2D * p2DTexture = nullptr;
	ID3D11ShaderResourceView* srvPtr = nullptr;
	D3D11_SUBRESOURCE_DATA initialData{};
	initialData.pSysMem = colorData;
	initialData.SysMemPitch = width * sizeof(Color);

	HRESULT hr = Module::GetDevice().CreateTexture2D(&textureDesc, &initialData, &p2DTexture);
	COM_ERROR_IF_FAILED(hr, "Failed to initialize texture from color data.");

	texture = Microsoft::WRL::ComPtr<ID3D11Resource>(static_cast<ID3D11Texture2D*>(p2DTexture));
	CD3D11_SHADER_RESOURCE_VIEW_DESC srvDesc(D3D11_SRV_DIMENSION_TEXTURE2D, textureDesc.Format); //텍스쳐로부터 셰이더 리소스 뷰 생성
	
	hr = Module::GetDevice().CreateShaderResourceView(texture.Get(), &srvDesc, &srvPtr);
	COM_ERROR_IF_FAILED(hr, "Failed to create shader resource view from texture generated from color data.");

	textureView = Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>(srvPtr);
}