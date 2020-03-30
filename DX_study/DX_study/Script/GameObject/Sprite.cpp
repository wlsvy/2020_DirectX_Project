#include "Sprite.h"
#include "../Internal/Core/InternalHelper.h"

bool Sprite::Initialize(
	float width, 
	float height, 
	std::string spritePath, 
	ConstantBuffer<CB_VS_vertexshader_2d> & cb_vs_vertexshader_2d)
{
	this->cb_vs_vertexshader_2d = &cb_vs_vertexshader_2d;

	texture = std::make_unique<Texture>(spritePath, aiTextureType::aiTextureType_DIFFUSE);

	std::vector<Vertex2D> vertexData = {
		Vertex2D(-0.5f, -0.5f, 0.0f, 0.0f, 0.0f), //TopLeft
		Vertex2D(0.5f, -0.5f, 0.0f, 1.0f, 0.0f), //TopRight
		Vertex2D(-0.5, 0.5, 0.0f, 0.0f, 1.0f), //Bottom Left
		Vertex2D(0.5, 0.5, 0.0f, 1.0f, 1.0f), //Bottom Right
	};

	std::vector<DWORD> indexData = {
		0, 1, 2,
		2, 1, 3
	};

	HRESULT hr = vertices.Initialize(vertexData.data(), vertexData.size());
	COM_ERROR_IF_FAILED(hr, "Failed to initialize vertex buffer for sprite.");

	hr = indices.Initialize(indexData.data(), indexData.size());
	COM_ERROR_IF_FAILED(hr, "Failed to initialize index buffer for mesh.");

	SetPosition(0.0f, 0.0f, 0.0f);
	SetRotation(0.0f, 0.0f, 0.0f);

	SetScale(width, height);

	return true;
}

void Sprite::Draw(XMMATRIX orthoMatrix)
{
	XMMATRIX wvpMatrix = worldMatrix * orthoMatrix;
	Core::GetDeviceContext()->VSSetConstantBuffers(0, 1, cb_vs_vertexshader_2d->GetAddressOf());
	cb_vs_vertexshader_2d->data.wvpMatrix = wvpMatrix;
	cb_vs_vertexshader_2d->ApplyChanges();

	Core::GetDeviceContext()->PSSetShaderResources(0, 1, texture->GetTextureResourceViewAddress());

	const UINT offsets = 0;
	Core::GetDeviceContext()->IASetVertexBuffers(0, 1, vertices.GetAddressOf(), vertices.StridePtr(), &offsets);
	Core::GetDeviceContext()->IASetIndexBuffer(indices.Get(), DXGI_FORMAT::DXGI_FORMAT_R32_UINT, 0);
	Core::GetDeviceContext()->DrawIndexed(indices.IndexCount(), 0, 0);
}

float Sprite::GetWidth()
{
	return scale.x;
}

float Sprite::GetHeight()
{
	return scale.y;
}

void Sprite::UpdateMatrix()
{
	worldMatrix = XMMatrixScaling(scale.x, scale.y, scale.z) * XMMatrixRotationRollPitchYaw(rot.x, rot.y, rot.z) * XMMatrixTranslation(pos.x + scale.x / 2.0f, pos.y + scale.y / 2.0f, pos.z);
}
