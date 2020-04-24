#include "Shaders.h"
#include "../Core/InternalHelper.h"
#include "../Core/ObjectPool.h"
#include "../../Util/StringHelper.h"

std::shared_ptr<VertexShader> VertexShader::GetDefault() {
	return Core::Find<VertexShader>("vertexshader");
}

bool VertexShader::Initialize(const std::string & shaderpath, D3D11_INPUT_ELEMENT_DESC * layoutDesc, UINT numElements) {
	Name = StringHelper::GetFileNameFromPath(shaderpath);
	auto wstr = StringHelper::StringToWide(shaderpath);

	HRESULT hr = D3DReadFileToBlob(wstr.c_str(), this->shader_buffer.GetAddressOf());
	if (FAILED(hr)) {
		std::wstring errorMsg = L"Failed to load shader: " + wstr;
		ErrorLogger::Log(hr, errorMsg);
		return false;
	}

	hr = Core::GetDevice()->CreateVertexShader(this->shader_buffer->GetBufferPointer(), this->shader_buffer->GetBufferSize(), NULL, this->shader.GetAddressOf());
	if (FAILED(hr)) {
		std::wstring errorMsg = L"Failed to create vertex shader: " + wstr;
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

std::shared_ptr<PixelShader> PixelShader::GetDefault() {
	return Core::Find<PixelShader>("pixelshader_deferred");
}

bool PixelShader::Initialize(const std::string & shaderpath) {
	Name = StringHelper::GetFileNameFromPath(shaderpath);
	auto wstr = StringHelper::StringToWide(shaderpath);
	HRESULT hr = D3DReadFileToBlob(wstr.c_str(), this->shader_buffer.GetAddressOf());

	if (FAILED(hr)) {
		std::wstring errorMsg = L"Failed to load shader: " + wstr;
		ErrorLogger::Log(hr, errorMsg);
		return false;
	}

	hr = Core::GetDevice()->CreatePixelShader(this->shader_buffer->GetBufferPointer(), this->shader_buffer->GetBufferSize(), NULL, this->shader.GetAddressOf());
	if (FAILED(hr)) {
		std::wstring errorMsg = L"Failed to create vertex shader: " + wstr;
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