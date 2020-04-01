#pragma once
#include <vector>
#include <memory>
#include <unordered_map>

#include "../Engine/AdapterReader.h"
#include "Shaders.h"

#include "../../GameObject/Camera.h"
#include "../../GameObject/RenderableGameObject.h"
#include "../../GameObject/Light.h"
#include "../../GameObject/Sprite.h"

class BufferBase;
class Texture;
class Material;
class Model;

class Graphics {
public:
	Graphics();
	bool Initialize(HWND hwnd, int width, int height);

	void RenderFrame();

	void DrawFrameString();
	void DrawImGui();

	Camera3D Camera3D;
	Sprite sprite;
	RenderableGameObject gameObject;
	Light light;

private:
	bool InitializeShaders();
	bool InitializeScene();

	int windowWidth = 0;
	int windowHeight = 0;

	VertexShader vertexshader;
	VertexShader vertexshader_2d;
	PixelShader pixelshader;
	PixelShader pixelshader_2d;
	PixelShader pixelshader_2d_discard;
	PixelShader pixelshader_nolight;

	std::unique_ptr<ConstantBuffer<CB_VS_vertexshader_2d>> cb_vs_vertexshader_2d;
	std::unique_ptr<ConstantBuffer<CB_VS_vertexshader>> cb_vs_vertexshader;
	std::unique_ptr<ConstantBuffer<CB_PS_light>> cb_ps_light;

	std::unordered_map<std::string, std::shared_ptr<BufferBase>> m_Buffers;
	std::unordered_map<std::string, std::shared_ptr<Texture>> m_Textures;
	std::unordered_map<std::string, std::shared_ptr<Material>> m_Materials;
	std::unordered_map<std::string, std::shared_ptr<Model>> m_Model;
};