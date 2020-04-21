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
class SkinnedModel;
class Mesh;
class Renderable;
class Engine;
class Camera;
class Light;
class GameObject;
class Skybox;
class Animator;
class VertexShader;
class PixelShader;

class Graphics {
public:
	bool Initialize(HWND hwnd, int width, int height);

	void RenderFrame();
	void Draw(const std::shared_ptr<Renderable>& renderer);
	void Draw_OtherMethod(const std::shared_ptr<Renderable>& renderer);
	void DrawModel(
		const std::shared_ptr<Model>& model,
		const DirectX::XMMATRIX & worldMat, 
		const DirectX::XMMATRIX & wvpMat);
	void DrawModel(
		const std::shared_ptr<SkinnedModel>& model,
		const DirectX::XMMATRIX & worldMat,
		const DirectX::XMMATRIX & wvpMat);
	void DrawMesh(const Mesh & mesh,
		const DirectX::XMMATRIX & worldMat, 
		const DirectX::XMMATRIX & wvpMat);
	void DebugDraw(const std::shared_ptr<Renderable>& renderer);
	void PostProcess();
	void DrawGui();
	void DrawSkybox();
	void DrawGuiDebug();
	void SetRenderTarget(ID3D11RenderTargetView* const* rtv, int bufferCount = 1);
	void SwapBuffer();

	DeviceResources & GetDeviceResources() { return m_DeviceResources; }
	ConstantBuffer<CB_VS_vertexshader_2d> & GetCbVertexShader2D() { return cb_vs_vertexshader_2d; }
	ConstantBuffer<CB_VS_vertexshader> & GetCbVertexShader() { return cb_vs_vertexshader; }
	ConstantBuffer<CB_PS_light> & GetCbPsLight() { return cb_ps_light; }
	ConstantBuffer<CB_VS_boneData> & GetCbBoneInfo() { return cb_BoneInfo; }

	int GetWindowWidth() const { return windowWidth; }
	int GetWindowHeight() const { return windowHeight; }

private:
	DeviceResources m_DeviceResources;

	ConstantBuffer<CB_VS_vertexshader_2d> cb_vs_vertexshader_2d;
	ConstantBuffer<CB_VS_vertexshader> cb_vs_vertexshader;
	ConstantBuffer<CB_PS_light> cb_ps_light;
	ConstantBuffer<CB_VS_boneData> cb_BoneInfo;

	std::shared_ptr<Skybox> m_Skybox;
	std::shared_ptr<VertexShader> m_PostProcesVshader;
	std::shared_ptr<PixelShader> m_PostProcesPshader;
	std::shared_ptr<Model> m_PostProcesWindowModel;

	int windowWidth = 0;
	int windowHeight = 0;
	const float m_BackgroundColor[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
	const float m_BlendFactors[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
};