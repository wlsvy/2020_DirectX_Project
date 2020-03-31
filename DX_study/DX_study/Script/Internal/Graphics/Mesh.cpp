#include "Mesh.h"

Mesh::Mesh(
	std::vector<Vertex3D>& vertices, 
	std::vector<DWORD>& indices, 
	std::vector<Texture> & textures, 
	const DirectX::XMMATRIX & transformMatrix)
{
	this->textures = textures;
	this->transformMatrix = transformMatrix;

	try {
		HRESULT hr = this->vertexbuffer.Initialize(vertices.data(), vertices.size());
		ThrowIfFailed(hr, "Failed to initialize vertex buffer for mesh.");

		hr = this->indexbuffer.Initialize(indices.data(), indices.size());
		ThrowIfFailed(hr, "Failed to initialize index buffer for mesh.");
	}
	catch (COMException e) {
		
	}
	
}

Mesh::Mesh(const Mesh & mesh)
{
	this->indexbuffer = mesh.indexbuffer;
	this->vertexbuffer = mesh.vertexbuffer;
	this->textures = mesh.textures;
	this->transformMatrix = mesh.transformMatrix;
}

void Mesh::Draw()
{
	UINT offset = 0;

	for (int i = 0; i < textures.size(); i++) {
		if (textures[i].GetType() == aiTextureType::aiTextureType_DIFFUSE) {
			Core::GetDeviceContext()->PSSetShaderResources(0, 1, textures[i].GetTextureResourceViewAddress());
			break;
		}
	}

	Core::GetDeviceContext()->IASetVertexBuffers(0, 1, this->vertexbuffer.GetAddressOf(), this->vertexbuffer.StridePtr(), &offset);
	Core::GetDeviceContext()->IASetIndexBuffer(this->indexbuffer.Get(), DXGI_FORMAT::DXGI_FORMAT_R32_UINT, 0);
	Core::GetDeviceContext()->DrawIndexed(this->indexbuffer.IndexCount(), 0, 0);

}

const DirectX::XMMATRIX & Mesh::GetTransformMatrix()
{
	return this->transformMatrix;
}
