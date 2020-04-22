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
public:
	MANAGED_OBJECT(Texture)

	Texture(const Color4Byte & color, aiTextureType type);
	Texture(const Color4Byte * colorData, UINT width, UINT height, aiTextureType type);
	Texture(const std::string & filePath, aiTextureType type);
	Texture(const uint8_t* pData, size_t size, aiTextureType type);
	Texture(const Texture&);
	Texture(Texture &&);
	Texture& operator=(const Texture &);

	const aiTextureType GetType()										const { return type; }
	ID3D11ShaderResourceView *			GetTextureResourceView()		const { return textureView.Get(); }
	ID3D11ShaderResourceView * const *	GetTextureResourceViewAddress()	const { return textureView.GetAddressOf(); }

private:
	void Initialize1x1ColorTexture(const Color4Byte & colorData, aiTextureType type);
	void InitializeColorTexture(const Color4Byte * colorData, UINT width, UINT height, aiTextureType type);

	Microsoft::WRL::ComPtr<ID3D11Resource> texture = nullptr;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> textureView = nullptr;
	aiTextureType type = aiTextureType::aiTextureType_UNKNOWN;
};