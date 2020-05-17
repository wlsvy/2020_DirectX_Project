#include "Texture.h"
#include "../../Util/ErrorLogger.h"
#include <WICTextureLoader.h>
#include <DDSTextureLoader.h>

#include "../Core/InternalHelper.h"
#include "../Core/ObjectPool.h"
#include "../../Util/StringHelper.h"

std::shared_ptr<Texture> Texture::GetDefault() {
	static std::weak_ptr<Texture> s_Default = Core::Find<Texture>("WhiteTexture");
	return s_Default.lock();
}
std::shared_ptr<Texture> Texture::GetDefaultNormalMap()
{
	static std::weak_ptr<Texture> s_Default = Core::Find<Texture>("defaultnormalmap");
	return s_Default.lock();
}

Texture::Texture(const Color4Byte & color)
{
	Initialize1x1DeferredRenderingResource2(color);
}

Texture::Texture(const Color4Byte * colorData, UINT width, UINT height)
{
	InitializeDeferredRenderingResource2(colorData, width, height);
}


Texture::Texture(const std::string & filePath) : Object(StringHelper::GetFileNameFromPath(filePath))
{
	Microsoft::WRL::ComPtr<ID3D11Resource> resource;

	HRESULT hr = DirectX::CreateWICTextureFromFile(
		Core::GetDevice(), 
		StringHelper::StringToWide(filePath).c_str(), 
		resource.GetAddressOf(),
		m_ShaderResourceView.GetAddressOf());

	if (FAILED(hr))
	{
		Initialize1x1DeferredRenderingResource2(Color4Byte::UnloadedTextureColor);
	}
}

Texture::Texture(const uint8_t * pData, size_t size) 
{
	Microsoft::WRL::ComPtr<ID3D11Resource> resource;
	ThrowIfFailed(
		DirectX::CreateWICTextureFromMemory(Core::GetDevice(), pData, size, resource.GetAddressOf(), this->m_ShaderResourceView.GetAddressOf()),
		"Failed to create Texture from.");
}

void Texture::Initialize1x1DeferredRenderingResource2(const Color4Byte & colorData)
{
	InitializeDeferredRenderingResource2(&colorData, 1, 1);
}

void Texture::InitializeDeferredRenderingResource2(const Color4Byte * colorData, UINT width, UINT height)
{
	Microsoft::WRL::ComPtr<ID3D11Texture2D> texture;

	CD3D11_TEXTURE2D_DESC textureDesc(DXGI_FORMAT_R8G8B8A8_UNORM, width, height);
	D3D11_SUBRESOURCE_DATA initialData{};
	initialData.pSysMem = colorData;
	initialData.SysMemPitch = width * sizeof(Color4Byte);

	ThrowIfFailed(
		Core::GetDevice()->CreateTexture2D(&textureDesc, &initialData, texture.GetAddressOf()),
		"Failed to initialize texture from color data.");

	CD3D11_SHADER_RESOURCE_VIEW_DESC srvDesc(D3D11_SRV_DIMENSION_TEXTURE2D, textureDesc.Format);
	ThrowIfFailed(
		Core::GetDevice()->CreateShaderResourceView(texture.Get(), &srvDesc, m_ShaderResourceView.GetAddressOf()),
		"Failed to create shader resource view from texture generated from color data.");
}