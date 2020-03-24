#ifndef VertexBuffer_h__
#define VertexBuffer_h__
#include <d3d11.h>
#include <wrl/client.h>
#include <memory>

template<class T>
class VertexBuffer
{
private:
	Microsoft::WRL::ComPtr<ID3D11Buffer> buffer;
	UINT stride = sizeof(T);
	UINT vertexCount = 0;

public:
	VertexBuffer() {}

	VertexBuffer(const VertexBuffer<T> & rhs) {
		this->buffer = rhs.buffer;
		this->vertexCount = rhs.vertexCount;
		this->stride = rhs.stride;
	}

	VertexBuffer<T> & operator=(const VertexBuffer<T> & a) {
		this->buffer = a.buffer;
		this->vertexCount = a.vertexCount;
		this->stride = a.stride;
		return *this;
	}

	ID3D11Buffer* Get()const
	{
		return buffer.Get();
	}

	ID3D11Buffer* const* GetAddressOf()const
	{
		return buffer.GetAddressOf();
	}

	UINT VertexCount() const
	{
		return this->vertexCount;
	}

	const UINT Stride() const
	{
		return this->stride;
	}

	const UINT * StridePtr() const
	{
		return &this->stride;
	}

	HRESULT Initialize(ID3D11Device *device, T * data, UINT vertexCount)
	{
		if (buffer.Get() != nullptr) buffer.Reset();

		this->vertexCount = vertexCount;

		D3D11_BUFFER_DESC vertexBufferDesc;
		ZeroMemory(&vertexBufferDesc, sizeof(vertexBufferDesc));

		vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
		vertexBufferDesc.ByteWidth = stride * vertexCount;
		vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		vertexBufferDesc.CPUAccessFlags = 0;
		vertexBufferDesc.MiscFlags = 0;

		/*버퍼 정의 구조체
		typedef struct D3D11_BUFFER_DESC
		 {
		UINT ByteWidth;			//생성할 정점 버퍼의 크기
		D3D11_USAGE Usage;		//버퍼가 쓰이는 방식. D3D11_USAGE_DEFAULT-> gpu가 자원을 읽고 쓴다. cpu는 사용 불가
		UINT BindFlags;			//정점 버퍼의 경우에는 D3D11_BIND_VERTEX_BUFFER를 지정한다.
		UINT CPUAccessFlags;	//cpu가 버퍼에 접근하는 방식에 관련된 플래그. cpu가 버퍼를 읽고 쓰지 않는다면 0
		UINT MiscFlags;			//정점 버퍼는 이 기타 플래그를 사용할 필요가 없다. 그냥 0
		UINT StructureByteStride;	//구조적 버퍼(structured buffer)에 지정된 원소 하나의 크기(바이트 단위),
		} 	D3D11_BUFFER_DESC;
		*/

		D3D11_SUBRESOURCE_DATA vertexBufferData;
		ZeroMemory(&vertexBufferData, sizeof(vertexBufferData));
		vertexBufferData.pSysMem = data;

		/*
		typedef struct D3D11_SUBRESOURCE_DATA
		{
		const void *pSysMem;			//정점 버퍼를 초기화할 자료를 담은 시스템 메모리 배열을 가리키는 포인터
		UINT SysMemPitch;				//정점 버퍼에는 쓰이지 않는다.
		UINT SysMemSlicePitch;			//정점 버퍼에는 쓰이지 않는다.
		} 	D3D11_SUBRESOURCE_DATA;
		*/

		HRESULT hr = device->CreateBuffer(&vertexBufferDesc, &vertexBufferData, this->buffer.GetAddressOf());
		return hr;
	}
	HRESULT Initialize_DynamicBuffer(ID3D11Device *device, T * data, UINT vertexCount) {
		if (buffer.Get() != nullptr) buffer.Reset();

		this->vertexCount = vertexCount;

		D3D11_BUFFER_DESC vertexBufferDesc;
		ZeroMemory(&vertexBufferDesc, sizeof(vertexBufferDesc));

		vertexBufferDesc.Usage = D3D11_USAGE_DYNAMIC;				//버퍼 값 변경 가능
		vertexBufferDesc.ByteWidth = stride * vertexCount;
		vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		vertexBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;	//cpu 접근 가능
		vertexBufferDesc.MiscFlags = 0;

		D3D11_SUBRESOURCE_DATA vertexBufferData;
		ZeroMemory(&vertexBufferData, sizeof(vertexBufferData));
		vertexBufferData.pSysMem = data;

		HRESULT hr = device->CreateBuffer(&vertexBufferDesc, &vertexBufferData, this->buffer.GetAddressOf());
		return hr;
	}
};

#endif // VertexBuffer_h__