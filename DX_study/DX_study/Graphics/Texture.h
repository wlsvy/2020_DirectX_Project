#pragma once
#include <d3d11.h>
#include <wrl/client.h>
#include "Color.h"
#include <string>
#include <assimp/material.h>


enum class TextureStorageType {
	Invalid,
	None,
	EmbeddedIndexCompressed,
	EmbeddedIndexNonCompressed,
	EmbeddedCompressed,
	EmbeddedNonCompressed,
	Disk
};

class Texture {
	friend class GraphicsManager;
public:
	Texture(const Color & color, aiTextureType type);
	Texture(const Color * colorData, UINT width, UINT height, aiTextureType type);
	Texture(const std::string & filePath, aiTextureType type);
	Texture(const uint8_t* pData, size_t size, aiTextureType type);

	Texture(const Texture & _rhs);
	Texture(Texture && _rhs);

	aiTextureType GetType();
	ID3D11ShaderResourceView * GetTextureResourceView();
	ID3D11ShaderResourceView ** GetTextureResourceViewAddress();

	std::string Name = "";
	//int ID = -1;

private:
	void Initialize1x1ColorTexture(const Color & colorData, aiTextureType type);
	void InitializeColorTexture(const Color * colorData, UINT width, UINT height, aiTextureType type);

	Microsoft::WRL::ComPtr<ID3D11Resource> texture = nullptr;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> textureView = nullptr;
	aiTextureType type = aiTextureType::aiTextureType_UNKNOWN;
};