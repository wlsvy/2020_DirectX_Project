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
		void *pData;		//�а� �� �� �ִ� �ڿ��� ��(raw) �޸� ����� ����Ű�� ������
		UINT RowPitch;		//�ڿ��� �ڷ� �� ���� ����Ʈ ���� ũ��
		UINT DepthPitch;	//�ڿ��� �ڷ� �� �������� ����Ʈ ���� ũ��.
		} 	D3D11_MAPPED_SUBRESOURCE;
		*/
		HRESULT hr = this->deviceContext->Map(buffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
		/*�������� ���۸� �����ϴ� �޼ҵ�. �⺻������ ������ cpu�� �ڿ��� gpu�� �޸𸮷� �����ϴ� ��. ������� �δ��� �ִ�.
		virtual HRESULT STDMETHODCALLTYPE Map( 
			_In_  ID3D11Resource *pResource,	//�б� ���� ������ ���ϴ� �ڿ��� ����Ű�� ������
			_In_  UINT Subresource,				//�� �ڿ��� ��� �κ� �ڿ��� ����
			_In_  D3D11_MAP MapType,			//�ڿ��鿡 ���� �÷��� �� ���� Ÿ���� ����->discard, overwrite, read
			_In_  UINT MapFlags,				//�߰� �÷���
			_Out_opt_  D3D11_MAPPED_SUBRESOURCE *pMappedResource) = 0;	//����ü�� ����Ű�� �����͸� �����ش�. �̸� ���ؼ� ���۸� �а� ����.
		*/
		if (FAILED(hr))
		{
			ErrorLogger::Log(hr, "Failed to map constant buffer.");
			return false;
		}
		CopyMemory(mappedResource.pData, &data, sizeof(T));
		this->deviceContext->Unmap(buffer.Get(), 0); //�ڿ��� ����Ű�� �����͸� ��Ȱ��ȭ ��Ű�� �ٽ� gpu�� ������ �����ϰ� �Ѵ�.
		return true;
	}
};

#endif // ConstantBuffer_h__