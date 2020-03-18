#include "Shaders.h"

bool VertexShader::Initialize(Microsoft::WRL::ComPtr<ID3D11Device> &device, std::wstring shaderpath, D3D11_INPUT_ELEMENT_DESC * layoutDesc, UINT numElements, const std::string & filename) {
	shaderName = filename;
	HRESULT hr = D3DReadFileToBlob(shaderpath.c_str(), this->shader_buffer.GetAddressOf()); //컴파일된 셰이더 파일을 버퍼로 읽어들이고

	if (FAILED(hr)) {
		std::wstring errorMsg = L"Failed to load shader: " + shaderpath;
		ErrorLogger::Log(hr, errorMsg);
		return false;
	}

	//셰이더 버퍼로부터 셰이더 데이터를 생성
	hr = device->CreateVertexShader(this->shader_buffer->GetBufferPointer(), this->shader_buffer->GetBufferSize(), NULL, this->shader.GetAddressOf());
	if (FAILED(hr)) {
		std::wstring errorMsg = L"Failed to create vertex shader: " + shaderpath;
		ErrorLogger::Log(hr, errorMsg);
		return false;
	}

	//SIZE_T testsize = shader_buffer->GetBufferSize();
	

	hr = device->CreateInputLayout(layoutDesc, numElements, this->shader_buffer->GetBufferPointer(), this->shader_buffer->GetBufferSize(), this->inputLayout.GetAddressOf());
	if (FAILED(hr)) {
		ErrorLogger::Log(hr, "Failed to create input layer.");
		return false;
	}
	/* //inputlayout를 생성하는 메소드.
	virtual HRESULT STDMETHODCALLTYPE CreateInputLayout( 
	_In_reads_(NumElements)  const D3D11_INPUT_ELEMENT_DESC *pInputElementDescs,		//정점 구조체를 서술하는 구조체 배열
		_In_range_(0, D3D11_IA_VERTEX_INPUT_STRUCTURE_ELEMENT_COUNT)  UINT NumElements,	//구조체 배열의 원소 개수
		_In_reads_(BytecodeLength)  const void *pShaderBytecodeWithInputSignature,	//정점 셰이더를 컴파일해서 얻은 바이트 코드를 가리키는 포인터
		_In_  SIZE_T BytecodeLength,		//그 바이트 코드의 크기(바이트 단위)
		_COM_Outptr_opt_  ID3D11InputLayout **ppInputLayout) = 0;	//생성된 입력 배치를 이 포인터를 통해서 들려준다.
	*/

	return true;
}

ID3D11VertexShader * VertexShader::GetShader() {
	return this->shader.Get();
}

ID3D10Blob * VertexShader::GetBuffer() {
	return this->shader_buffer.Get();
}

ID3D11InputLayout * VertexShader::GetInputLayout()
{
	return this->inputLayout.Get();
}

bool PixelShader::Initialize(Microsoft::WRL::ComPtr<ID3D11Device> &device, std::wstring shaderpath, const std::string & filename) {
	shaderName = filename;
	HRESULT hr = D3DReadFileToBlob(shaderpath.c_str(), this->shader_buffer.GetAddressOf());

	if (FAILED(hr)) {
		std::wstring errorMsg = L"Failed to load shader: " + shaderpath;
		ErrorLogger::Log(hr, errorMsg);
		return false;
	}

	hr = device->CreatePixelShader(this->shader_buffer->GetBufferPointer(), this->shader_buffer->GetBufferSize(), NULL, this->shader.GetAddressOf());
	if (FAILED(hr)) {
		std::wstring errorMsg = L"Failed to create vertex shader: " + shaderpath;
		ErrorLogger::Log(hr, errorMsg);
		return false;
	}

	return true;
}

ID3D11PixelShader * PixelShader::GetShader() {
	return this->shader.Get();
}

ID3D10Blob * PixelShader::GetBuffer() {
	return this->shader_buffer.Get();
}

bool GeometryShader::Initialize(Microsoft::WRL::ComPtr<ID3D11Device>& device, std::wstring shaderpath, const std::string & filename)
{
	shaderName = filename;
	HRESULT hr = D3DReadFileToBlob(shaderpath.c_str(), this->shader_buffer.GetAddressOf());

	if (FAILED(hr)) {
		std::wstring errorMsg = L"Failed to load shader: " + shaderpath;
		ErrorLogger::Log(hr, errorMsg);
		return false;
	}

	hr = device->CreateGeometryShader(this->shader_buffer->GetBufferPointer(), this->shader_buffer->GetBufferSize(), NULL, this->shader.GetAddressOf());
	if (FAILED(hr)) {
		std::wstring errorMsg = L"Failed to create vertex shader: " + shaderpath;
		ErrorLogger::Log(hr, errorMsg);
		return false;
	}

	return true;
}

ID3D11GeometryShader * GeometryShader::GetShader()
{
	return this->shader.Get();
}

ID3D10Blob * GeometryShader::GetBuffer()
{
	return this->shader_buffer.Get();
}

bool VertexInputLayout::Initialize(Microsoft::WRL::ComPtr<ID3D11Device>& device, D3D11_INPUT_ELEMENT_DESC * layoutDesc, UINT numElements)
{
	/*HRESULT hr = device->CreateInputLayout(layoutDesc, numElements, this->shader_buffer->GetBufferPointer(), this->shader_buffer->GetBufferSize(), this->inputLayout.GetAddressOf());
	if (FAILED(hr)) {
		ErrorLogger::Log(hr, "Failed to create input layer.");
		return false;
	}*/
	/* //inputlayout를 생성하는 메소드.
	virtual HRESULT STDMETHODCALLTYPE CreateInputLayout(
	_In_reads_(NumElements)  const D3D11_INPUT_ELEMENT_DESC *pInputElementDescs,		//정점 구조체를 서술하는 구조체 배열
		_In_range_(0, D3D11_IA_VERTEX_INPUT_STRUCTURE_ELEMENT_COUNT)  UINT NumElements,	//구조체 배열의 원소 개수
		_In_reads_(BytecodeLength)  const void *pShaderBytecodeWithInputSignature,	//정점 셰이더를 컴파일해서 얻은 바이트 코드를 가리키는 포인터
		_In_  SIZE_T BytecodeLength,		//그 바이트 코드의 크기(바이트 단위)
		_COM_Outptr_opt_  ID3D11InputLayout **ppInputLayout) = 0;	//생성된 입력 배치를 이 포인터를 통해서 들려준다.
	*/

	return true;
}

ID3D11InputLayout * VertexInputLayout::GetInputLayout()
{
	return nullptr;
}
