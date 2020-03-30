#pragma once
#include "AdapterReader.h"
#include "Shaders.h"

#include <SpriteBatch.h>
#include <SpriteFont.h>
#include "Camera.h"
#include "RenderableGameObject.h"
#include "Light.h"
#include "Sprite.h"

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

	VertexShader vertexshader;
	VertexShader vertexshader_2d;
	PixelShader pixelshader;
	PixelShader pixelshader_2d;
	PixelShader pixelshader_2d_discard;
	PixelShader pixelshader_nolight;
	ConstantBuffer<CB_VS_vertexshader_2d> cb_vs_vertexshader_2d;
	ConstantBuffer<CB_VS_vertexshader> cb_vs_vertexshader;
	ConstantBuffer<CB_PS_light> cb_ps_light;

	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> myTexture;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> pinkTexture;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> grassTexture;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> pavementTexture;

	int windowWidth = 0;
	int windowHeight = 0;
};