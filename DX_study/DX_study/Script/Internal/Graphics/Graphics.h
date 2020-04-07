#pragma once
#include <vector>
#include <memory>
#include <unordered_map>

#include "Shaders.h"
#include "ConstantBuffer.h"
#include "../Engine/DeviceResources.h"

class BufferBase;
class Texture;
class Material;
class Model;
class Renderable;
class Engine;
class Camera;
class Light;
class GameObject;

class Graphics {
public:
	bool Initialize(HWND hwnd, int width, int height);

	void RenderFrame();
	void Draw(const std::shared_ptr<Renderable>& renderer);
	void DrawImGui();
	void SetOmRenderTargetToBase();
	void SetOmRenderTargetToAux();
	void SwapBuffer();

	DeviceResources & GetDeviceResources() { return m_DeviceResources; }
	ConstantBuffer<CB_VS_vertexshader_2d> & GetCbVertexShader2D() { return cb_vs_vertexshader_2d; }
	ConstantBuffer<CB_VS_vertexshader> & GetCbVertexShader() { return cb_vs_vertexshader; }
	ConstantBuffer<CB_PS_light> & GetCbPsLight() { return cb_ps_light; }
	
	std::shared_ptr<Camera> mainCam;
	std::shared_ptr<GameObject> gameObject;
	std::shared_ptr<Light> light;

private:
	bool InitializeShaders();
	bool InitializeScene();
	

	

	DeviceResources m_DeviceResources;

	VertexShader vertexshader;
	VertexShader vertexshader_2d;
	PixelShader pixelshader;
	PixelShader pixelshader_2d;
	PixelShader pixelshader_2d_discard;
	PixelShader pixelshader_nolight;

	ConstantBuffer<CB_VS_vertexshader_2d> cb_vs_vertexshader_2d;
	ConstantBuffer<CB_VS_vertexshader> cb_vs_vertexshader;
	ConstantBuffer<CB_PS_light> cb_ps_light;

	std::unordered_map<std::string, std::shared_ptr<BufferBase>> m_Buffers;
	std::unordered_map<std::string, std::shared_ptr<Texture>> m_Textures;
	std::unordered_map<std::string, std::shared_ptr<Material>> m_Materials;
	std::unordered_map<std::string, std::shared_ptr<Model>> m_Model;

	int windowWidth = 0;
	int windowHeight = 0;
	const float m_BackgroundColor[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
	const float m_BlendFactors[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
};