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

	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> myTexture;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> pinkTexture;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> grassTexture;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> pavementTexture;

	std::unordered_map<std::string, std::shared_ptr<BufferBase>> m_Buffers;
	std::unordered_map<std::string, std::shared_ptr<Texture>> m_Textures;
	std::unordered_map<std::string, std::shared_ptr<Material>> m_Materials;
	std::unordered_map<std::string, std::shared_ptr<Model>> m_Model;
};