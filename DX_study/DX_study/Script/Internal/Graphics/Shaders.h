#pragma once
#include "../../Util/ErrorLogger.h"
#pragma comment(lib, "D3DCompiler.lib")
#include <d3d11.h>
#include <wrl/client.h>
#include <d3dcompiler.h>
#include <string>
#include "../Core/Object.h"

class ShaderBase : public Object {
public:
	MANAGED_OBJECT(ShaderBase)
};

class VertexShader : public ShaderBase {
public:
	bool Initialize(std::wstring shaderpath, D3D11_INPUT_ELEMENT_DESC * layoutDesc, UINT numElements);
	ID3D11VertexShader * GetShader();
	ID3D10Blob * GetBuffer();
	ID3D11InputLayout * GetInputLayout();
private:
	Microsoft::WRL::ComPtr<ID3D11VertexShader> shader;
	Microsoft::WRL::ComPtr<ID3D10Blob> shader_buffer;
	Microsoft::WRL::ComPtr<ID3D11InputLayout> inputLayout;
};

class PixelShader : public ShaderBase {
public:
	bool Initialize(std::wstring shaderpath);
	ID3D11PixelShader * GetShader();
	ID3D10Blob * GetBuffer();
private:
	Microsoft::WRL::ComPtr<ID3D11PixelShader> shader;
	Microsoft::WRL::ComPtr<ID3D10Blob> shader_buffer;

};