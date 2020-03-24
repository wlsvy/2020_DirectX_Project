#ifndef ConstantBuffer_h__
#define ConstantBuffer_h__
#include <d3d11.h>
#include "ConstantBufferType.h"
#include <wrl/client.h>
#include "..\\ErrorLogger.h"

template<class T>
class ConstantBuffer
{
private:
	ConstantBuffer(const ConstantBuffer<T>& rhs);

private:

	Microsoft::WRL::ComPtr<ID3D11Buffer> buffer;
	ID3D11DeviceContext * deviceContext = nullptr;

public:
	ConstantBuffer() {}

	T data;

	ID3D11Buffer* Get()const
	{
		return buffer.Get();
	}

	ID3D11Buffer* const* GetAddressOf()const
	{
		return buffer.GetAddressOf();
	}

	HRESULT Initialize(ID3D11Device *device, ID3D11DeviceContext * deviceContext)
	{
		this->deviceContext = deviceContext;

		D3D11_BUFFER_DESC desc;
		desc.Usage = D3D11_USAGE_DYNAMIC;
		desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		desc.MiscFlags = 0;
		desc.ByteWidth = static_cast<UINT>(sizeof(T) + (16 - (sizeof(T) % 16)));
		desc.StructureByteStride = 0;

		HRESULT hr = device->CreateBuffer(&desc, 0, buffer.GetAddressOf());
		return hr;
	}

	bool ApplyChanges()
	{
		D3D11_MAPPED_SUBRESOURCE mappedResource;
		/*
		typedef struct D3D11_MAPPED_SUBRESOURCE
		{
		void *pData;		//읽고 쓸 수 있는 자원의 생(raw) 메모리 블록을 가리키는 포인터
		UINT RowPitch;		//자원의 자료 한 줄의 바이트 단위 크기
		UINT DepthPitch;	//자원의 자료 한 페이지의 바이트 단위 크기.
		} 	D3D11_MAPPED_SUBRESOURCE;
		*/
		HRESULT hr = this->deviceContext->Map(buffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
		/*동적으로 버퍼를 갱신하는 메소드. 기본적으로 맵핑은 cpu의 자원을 gpu의 메모리로 전송하는 것. 어느정도 부담이 있다.
		virtual HRESULT STDMETHODCALLTYPE Map( 
			_In_  ID3D11Resource *pResource,	//읽기 쓰기 접근을 원하는 자원을 가리키는 포인터
			_In_  UINT Subresource,				//그 자원에 담긴 부분 자원의 색인
			_In_  D3D11_MAP MapType,			//자원들에 공통 플래그 세 가지 타입이 있음->discard, overwrite, read
			_In_  UINT MapFlags,				//추가 플래그
			_Out_opt_  D3D11_MAPPED_SUBRESOURCE *pMappedResource) = 0;	//구조체를 가리키는 포인터를 돌려준다. 이를 통해서 버퍼를 읽고 쓴다.
		*/
		if (FAILED(hr))
		{
			ErrorLogger::Log(hr, "Failed to map constant buffer.");
			return false;
		}
		CopyMemory(mappedResource.pData, &data, sizeof(T));
		this->deviceContext->Unmap(buffer.Get(), 0); //자원을 가리키는 포인터를 비활성화 시키고 다시 gpu의 접근을 가능하게 한다.
		return true;
	}
};

#endif // ConstantBuffer_h__