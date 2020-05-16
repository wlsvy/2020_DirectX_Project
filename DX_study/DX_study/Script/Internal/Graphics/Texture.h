#pragma once
#include <d3d11.h>
#include <wrl/client.h>
#include <string>
#include <assimp/material.h>
#include "../../Util/Color.h"
#include "../Core/Object.h"

enum class TextureStorageType {
	Invalid,
	None,
	EmbeddedIndexCompressed,
	EmbeddedIndexNonCompressed,
	EmbeddedCompressed,
	EmbeddedNonCompressed,
	Disk
};

class Texture : public Object {
	MANAGED_OBJECT(Texture)
public:
	static std::shared_ptr<Texture> GetDefault();
	static std::shared_ptr<Texture> GetDefaultNormalMap();

	Texture(const std::string & filePath);
	Texture(const Color4Byte & color);
	Texture(const Color4Byte * colorData, UINT width, UINT height);
	Texture(const uint8_t* pData, size_t size);

	ID3D11ShaderResourceView *			GetTextureResourceView()		const { return m_ShaderResourceView.Get(); }
	ID3D11ShaderResourceView * const *	GetTextureResourceViewAddress()	const { return m_ShaderResourceView.GetAddressOf(); }

private:
	void Initialize1x1ColorTexture(const Color4Byte & colorData);
	void InitializeColorTexture(const Color4Byte * colorData, UINT width, UINT height);

	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_ShaderResourceView = nullptr;
};