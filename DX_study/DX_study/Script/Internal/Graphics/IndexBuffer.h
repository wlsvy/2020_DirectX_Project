#pragma once
#include <d3d11.h>
#include <wrl/client.h>
#include <vector>

#include "../Core/InternalHelper.h"

class IndexBuffer {
private:
	Microsoft::WRL::ComPtr<ID3D11Buffer> buffer;
	UINT indexCount = 0;
public:
	ID3D11Buffer* Get() const {
		return buffer.Get();
	}

	ID3D11Buffer * const* GetAddressOf() const {
		return buffer.GetAddressOf();
	}

	UINT IndexCount() const {
		return this->indexCount;
	}

	HRESULT Initialize(DWORD * data, UINT indexCount) {
		this->indexCount = indexCount;
		//Load Index Data
		D3D11_BUFFER_DESC indexBufferDesc;
		ZeroMemory(&indexBufferDesc, sizeof(indexBufferDesc));
		indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
		indexBufferDesc.ByteWidth = sizeof(DWORD) * indexCount;
		indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
		indexBufferDesc.CPUAccessFlags = 0;
		indexBufferDesc.MiscFlags = 0;

		D3D11_SUBRESOURCE_DATA indexBufferData;
		indexBufferData.pSysMem = data;
		HRESULT hr = Core::GetDevice()->CreateBuffer(&indexBufferDesc, &indexBufferData, buffer.GetAddressOf());
		return hr;
	}
};