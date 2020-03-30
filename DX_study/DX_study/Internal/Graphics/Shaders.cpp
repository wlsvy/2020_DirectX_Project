#include "Shaders.h"
#include "../Core.h"

bool VertexShader::Initialize(std::wstring shaderpath, D3D11_INPUT_ELEMENT_DESC * layoutDesc, UINT numElements) {
	HRESULT hr = D3DReadFileToBlob(shaderpath.c_str(), this->shader_buffer.GetAddressOf());

	if (FAILED(hr)) {
		std::wstring errorMsg = L"Failed to load shader: " + shaderpath;
		ErrorLogger::Log(hr, errorMsg);
		return false;
	}

	hr = Core::GetDevice()->CreateVertexShader(this->shader_buffer->GetBufferPointer(), this->shader_buffer->GetBufferSize(), NULL, this->shader.GetAddressOf());
	if (FAILED(hr)) {
		std::wstring errorMsg = L"Failed to create vertex shader: " + shaderpath;
		ErrorLogger::Log(hr, errorMsg);
		return false;
	}

	hr = Core::GetDevice()->CreateInputLayout(layoutDesc, numElements, this->shader_buffer->GetBufferPointer(), this->shader_buffer->GetBufferSize(), this->inputLayout.GetAddressOf());
	if (FAILED(hr)) {
		ErrorLogger::Log(hr, "Failed to create input layer.");
		return false;
	}

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

bool PixelShader::Initialize(std::wstring shaderpath) {
	HRESULT hr = D3DReadFileToBlob(shaderpath.c_str(), this->shader_buffer.GetAddressOf());

	if (FAILED(hr)) {
		std::wstring errorMsg = L"Failed to load shader: " + shaderpath;
		ErrorLogger::Log(hr, errorMsg);
		return false;
	}

	hr = Core::GetDevice()->CreatePixelShader(this->shader_buffer->GetBufferPointer(), this->shader_buffer->GetBufferSize(), NULL, this->shader.GetAddressOf());
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