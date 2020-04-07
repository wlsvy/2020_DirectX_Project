#pragma once
#include <vector>
#include <memory>
#include <unordered_map>

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
class Skybox;

class Graphics {
public:
	bool Initialize(HWND hwnd, int width, int height);

	void RenderFrame();
	void Draw(const std::shared_ptr<Renderable>& renderer);
	void DrawUI();
	void DrawSkybox();
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
	bool InitializeScene();
	void LoadVertexShader(const std::string & filePath);
	void LoadPixelShader(const std::string & filePath);
	void LoadModel(const std::string & filePath);
	void LoadTexture(const std::string & filePath);
	
	DeviceResources m_DeviceResources;

	ConstantBuffer<CB_VS_vertexshader_2d> cb_vs_vertexshader_2d;
	ConstantBuffer<CB_VS_vertexshader> cb_vs_vertexshader;
	ConstantBuffer<CB_PS_light> cb_ps_light;

	std::shared_ptr<Skybox> m_Skybox;

	int windowWidth = 0;
	int windowHeight = 0;
	const float m_BackgroundColor[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
	const float m_BlendFactors[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
};