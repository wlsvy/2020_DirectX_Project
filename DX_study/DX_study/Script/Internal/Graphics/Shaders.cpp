#include "Shaders.h"

#pragma comment(lib, "D3DCompiler.lib")
#include <d3dcompiler.h>
#include "../Core/InternalHelper.h"
#include "../Core/ObjectPool.h"
#include "../../Util/StringHelper.h"

std::shared_ptr<VertexShader> VertexShader::GetDefault() {
	static std::weak_ptr<VertexShader> s_Default = Core::Find<VertexShader>("vertexshader");
	return s_Default.lock();
}

bool VertexShader::Initialize(const std::string & shaderpath, D3D11_INPUT_ELEMENT_DESC * layoutDesc, UINT numElements) {
	Name = StringHelper::GetFileNameFromPath(shaderpath);
	auto wstr = StringHelper::StringToWide(shaderpath);

	HRESULT hr = D3DReadFileToBlob(wstr.c_str(), this->m_ShaderBuffer.GetAddressOf());
	if (FAILED(hr)) {
		std::wstring errorMsg = L"Failed to load shader: " + wstr;
		StringHelper::ErrorLog(hr, errorMsg);
		return false;
	}

	hr = Core::GetDevice()->CreateVertexShader(this->m_ShaderBuffer->GetBufferPointer(), this->m_ShaderBuffer->GetBufferSize(), NULL, this->m_Shader.GetAddressOf());
	if (FAILED(hr)) {
		std::wstring errorMsg = L"Failed to create vertex shader: " + wstr;
		StringHelper::ErrorLog(hr, errorMsg);
		return false;
	}

	hr = Core::GetDevice()->CreateInputLayout(layoutDesc, numElements, this->m_ShaderBuffer->GetBufferPointer(), this->m_ShaderBuffer->GetBufferSize(), this->inputLayout.GetAddressOf());
	if (FAILED(hr)) {
		StringHelper::ErrorLog(hr, "Failed to create input layer.");
		return false;
	}

	return true;
}

std::shared_ptr<PixelShader> PixelShader::GetDefault() {
	static std::weak_ptr<PixelShader> s_Default = Core::Find<PixelShader>("PerObject");
	return s_Default.lock();
}

bool PixelShader::Initialize(const std::string & shaderpath) {
	Name = StringHelper::GetFileNameFromPath(shaderpath);
	auto wstr = StringHelper::StringToWide(shaderpath);
	HRESULT hr = D3DReadFileToBlob(wstr.c_str(), this->m_ShaderBuffer.GetAddressOf());

	if (FAILED(hr)) {
		std::wstring errorMsg = L"Failed to load shader: " + wstr;
		StringHelper::ErrorLog(hr, errorMsg);
		return false;
	}

	hr = Core::GetDevice()->CreatePixelShader(this->m_ShaderBuffer->GetBufferPointer(), this->m_ShaderBuffer->GetBufferSize(), NULL, this->m_Shader.GetAddressOf());
	if (FAILED(hr)) {
		std::wstring errorMsg = L"Failed to create Pixel shader: " + wstr;
		StringHelper::ErrorLog(hr, errorMsg);
		return false;
	}

	return true;
}

bool ComputeShader::Initialize(const std::string & shaderpath) {
	Name = StringHelper::GetFileNameFromPath(shaderpath);
	auto wstr = StringHelper::StringToWide(shaderpath);
	HRESULT hr = D3DReadFileToBlob(wstr.c_str(), this->m_ShaderBuffer.GetAddressOf());

	if (FAILED(hr)) {
		std::wstring errorMsg = L"Failed to load shader: " + wstr;
		StringHelper::ErrorLog(hr, errorMsg);
		return false;
	}

	hr = Core::GetDevice()->CreateComputeShader(m_ShaderBuffer->GetBufferPointer(), m_ShaderBuffer->GetBufferSize(), NULL, m_Shader.GetAddressOf());
	if (FAILED(hr)) {
		std::wstring errorMsg = L"Failed to create Compute shader: " + wstr;
		StringHelper::ErrorLog(hr, errorMsg);
		return false;
	}

	return true;
}

std::shared_ptr<GeometryShader> GeometryShader::GetDefault() {
	static std::weak_ptr<GeometryShader> s_Default = Core::Find<GeometryShader>("Default_gs");
	return s_Default.lock();
}

bool GeometryShader::Initialize(const std::string & shaderpath) {
	Name = StringHelper::GetFileNameFromPath(shaderpath);
	auto wstr = StringHelper::StringToWide(shaderpath);
	HRESULT hr = D3DReadFileToBlob(wstr.c_str(), this->m_ShaderBuffer.GetAddressOf());

	if (FAILED(hr)) {
		std::wstring errorMsg = L"Failed to load shader: " + wstr;
		StringHelper::ErrorLog(hr, errorMsg);
		return false;
	}

	hr = Core::GetDevice()->CreateGeometryShader(m_ShaderBuffer->GetBufferPointer(), m_ShaderBuffer->GetBufferSize(), NULL, m_Shader.GetAddressOf());
	if (FAILED(hr)) {
		std::wstring errorMsg = L"Failed to create Geometry shader: " + wstr;
		StringHelper::ErrorLog(hr, errorMsg);
		return false;
	}

	return true;
}