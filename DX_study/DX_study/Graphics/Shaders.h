#pragma once
#include "../ErrorLogger.h"
#pragma comment(lib, "D3DCompiler.lib")
#include <d3d11.h>
#include <wrl/client.h>
#include <d3dcompiler.h>
#include <string>

class VertexShader {
public:
	bool Initialize(Microsoft::WRL::ComPtr<ID3D11Device> &device, std::wstring shaderpath, D3D11_INPUT_ELEMENT_DESC * layoutDesc, UINT numElements, const std::string & filename);
	ID3D11VertexShader * GetShader();
	ID3D10Blob * GetBuffer();
	ID3D11InputLayout * GetInputLayout();

	std::string Name;
private:
	Microsoft::WRL::ComPtr<ID3D11VertexShader> m_Shader;
	Microsoft::WRL::ComPtr<ID3D10Blob> m_ShaderBuffer;
	Microsoft::WRL::ComPtr<ID3D11InputLayout> m_InputLayout;
};

class VertexInputLayout {
public:
	bool Initialize(Microsoft::WRL::ComPtr<ID3D11Device> &device, D3D11_INPUT_ELEMENT_DESC * layoutDesc, UINT numElements);
	ID3D11InputLayout * GetInputLayout();
private:
	Microsoft::WRL::ComPtr<ID3D11InputLayout> m_InputLayout;
};

class PixelShader {
public:
	bool Initialize(Microsoft::WRL::ComPtr<ID3D11Device> &device, std::wstring shaderpath, const std::string & filename);
	ID3D11PixelShader * GetShader();
	ID3D10Blob * GetBuffer();

	std::string Name;
private:
	Microsoft::WRL::ComPtr<ID3D11PixelShader> m_Shader;
	Microsoft::WRL::ComPtr<ID3D10Blob> m_ShaderBuffer;

};

class GeometryShader {
public:
	bool Initialize(Microsoft::WRL::ComPtr<ID3D11Device> &device, std::wstring shaderpath, const std::string & filename);
	ID3D11GeometryShader * GetShader();
	ID3D10Blob * GetBuffer();

	std::string Name;
private:
	Microsoft::WRL::ComPtr<ID3D11GeometryShader> m_Shader;
	Microsoft::WRL::ComPtr<ID3D10Blob> m_ShaderBuffer;

};