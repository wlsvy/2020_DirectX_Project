#include "Shaders.h"

#pragma comment(lib, "D3DCompiler.lib")
#include <d3dcompiler.h>
#include "../Core/InternalHelper.h"
#include "../Core/ObjectPool.h"
#include "../../Util/StringHelper.h"

std::shared_ptr<VertexShader> VertexShader::GetDefault() {
	return Core::Find<VertexShader>("vertexshader");
}

bool VertexShader::Initialize(const std::string & shaderpath, D3D11_INPUT_ELEMENT_DESC * layoutDesc, UINT numElements) {
	Name = StringHelper::GetFileNameFromPath(shaderpath);
	auto wstr = StringHelper::StringToWide(shaderpath);

	HRESULT hr = D3DReadFileToBlob(wstr.c_str(), this->m_ShaderBuffer.GetAddressOf());
	if (FAILED(hr)) {
		std::wstring errorMsg = L"Failed to load shader: " + wstr;
		ErrorLogger::Log(hr, errorMsg);
		return false;
	}

	hr = Core::GetDevice()->CreateVertexShader(this->m_ShaderBuffer->GetBufferPointer(), this->m_ShaderBuffer->GetBufferSize(), NULL, this->m_Shader.GetAddressOf());
	if (FAILED(hr)) {
		std::wstring errorMsg = L"Failed to create vertex shader: " + wstr;
		ErrorLogger::Log(hr, errorMsg);
		return false;
	}

	hr = Core::GetDevice()->CreateInputLayout(layoutDesc, numElements, this->m_ShaderBuffer->GetBufferPointer(), this->m_ShaderBuffer->GetBufferSize(), this->inputLayout.GetAddressOf());
	if (FAILED(hr)) {
		ErrorLogger::Log(hr, "Failed to create input layer.");
		return false;
	}

	return true;
}

std::shared_ptr<PixelShader> PixelShader::GetDefault() {
	return Core::Find<PixelShader>("pixelshader_deferred");
}

bool PixelShader::Initialize(const std::string & shaderpath) {
	Name = StringHelper::GetFileNameFromPath(shaderpath);
	auto wstr = StringHelper::StringToWide(shaderpath);
	HRESULT hr = D3DReadFileToBlob(wstr.c_str(), this->m_ShaderBuffer.GetAddressOf());

	if (FAILED(hr)) {
		std::wstring errorMsg = L"Failed to load shader: " + wstr;
		ErrorLogger::Log(hr, errorMsg);
		return false;
	}

	hr = Core::GetDevice()->CreatePixelShader(this->m_ShaderBuffer->GetBufferPointer(), this->m_ShaderBuffer->GetBufferSize(), NULL, this->m_Shader.GetAddressOf());
	if (FAILED(hr)) {
		std::wstring errorMsg = L"Failed to create Pixel shader: " + wstr;
		ErrorLogger::Log(hr, errorMsg);
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
		ErrorLogger::Log(hr, errorMsg);
		return false;
	}

	hr = Core::GetDevice()->CreateComputeShader(m_ShaderBuffer->GetBufferPointer(), m_ShaderBuffer->GetBufferSize(), NULL, m_Shader.GetAddressOf());
	if (FAILED(hr)) {
		std::wstring errorMsg = L"Failed to create Compute shader: " + wstr;
		ErrorLogger::Log(hr, errorMsg);
		return false;
	}

	return true;
}
