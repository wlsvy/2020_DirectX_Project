#pragma once
#include "../../Util/ErrorLogger.h"

#include <d3d11.h>
#include <wrl/client.h>
#include <string>
#include <memory>
#include "../Core/Object.h"

class VertexShader : public Object {
	MANAGED_OBJECT(VertexShader)
public:
	static std::shared_ptr<VertexShader> GetDefault();

	bool Initialize(const std::string & shaderpath, D3D11_INPUT_ELEMENT_DESC * layoutDesc, UINT numElements);
	ID3D11VertexShader * GetShader()		{ return m_Shader.Get(); }
	ID3D10Blob * GetBuffer()				{ return m_ShaderBuffer.Get(); }
	ID3D11InputLayout * GetInputLayout()	{ return inputLayout.Get(); }
private:
	Microsoft::WRL::ComPtr<ID3D11VertexShader> m_Shader;
	Microsoft::WRL::ComPtr<ID3D10Blob> m_ShaderBuffer;
	Microsoft::WRL::ComPtr<ID3D11InputLayout> inputLayout;
};

class PixelShader : public Object {
	MANAGED_OBJECT(PixelShader)
public:
	static std::shared_ptr<PixelShader> GetDefault();

	bool Initialize(const std::string & shaderpath);
	ID3D11PixelShader * GetShader()		{ return m_Shader.Get(); }
	ID3D10Blob * GetBuffer()			{ return m_ShaderBuffer.Get(); }
private:
	Microsoft::WRL::ComPtr<ID3D11PixelShader> m_Shader;
	Microsoft::WRL::ComPtr<ID3D10Blob> m_ShaderBuffer;

};

class ComputeShader : public Object {
	MANAGED_OBJECT(ComputeShader)
public:
	bool Initialize(const std::string & shaderpath);
	ID3D11ComputeShader * GetShader()	{ return m_Shader.Get(); }
	ID3D10Blob * GetBuffer()			{ return m_ShaderBuffer.Get(); }
private:
	Microsoft::WRL::ComPtr<ID3D11ComputeShader> m_Shader;
	Microsoft::WRL::ComPtr<ID3D10Blob> m_ShaderBuffer;

};