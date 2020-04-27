#pragma once
#include <vector>
#include <memory>
#include <unordered_map>
#include "ConstantBuffer.h"
#include "../Engine/DeviceResources.h"
#include "../../Util/Math.h"

class BufferBase;
class Texture;
class SharedMaterial;
class Model;
class SkinnedModel;
class Mesh;
class MeshBase;
class RenderInfo;
class Engine;
class Camera;
class Light;
class GameObject;
class Skybox;
class Animator;
class VertexShader;
class PixelShader;
class Material;
class LightBase;

class Graphics {
public:
	bool Initialize(HWND hwnd, int width, int height);
	bool ProcessMaterialTable();

	void RenderBegin();
	void RenderModels();
	void Render(const std::shared_ptr<RenderInfo>& renderer);
	void DrawMesh(const std::shared_ptr<MeshBase>& mesh,
		const DirectX::XMMATRIX & worldMat, 
		const DirectX::XMMATRIX & wvpMat);
	void DebugDraw(const std::shared_ptr<RenderInfo>& renderer);
	
	void PostProcess();
	void DrawGui();
	void DrawShadowMap(const std::shared_ptr<LightBase> & light);
	void DrawSkybox();
	void DrawGuiDebug();
	void SetRenderTarget(ID3D11RenderTargetView* const* rtv, int bufferCount = 1);
	void RenderEnd();

	DeviceResources & GetDeviceResources() { return m_DeviceResources; }
	ConstantBuffer<CB_VS_vertexshader_2d> & GetCbVertexShader2D() { return cb_vs_vertexshader_2d; }
	ConstantBuffer<CB_VS_vertexshader> & GetCbVertexShader() { return cb_vs_vertexshader; }
	ConstantBuffer<CB_PS_light> & GetCbPsLight() { return cb_ps_light; }
	ConstantBuffer<CB_VS_boneData> & GetCbBoneInfo() { return cb_vs_BoneInfo; }

	int GetWindowWidth() const { return windowWidth; }
	int GetWindowHeight() const { return windowHeight; }

private:
	void ApplyMaterialProperties(const std::shared_ptr<Material>& material);
	void ApplySkinnedBone(const std::shared_ptr<RenderInfo>& renderer);
	bool ViewFrustumCull(const std::shared_ptr<RenderInfo>& renderer);

	DeviceResources m_DeviceResources;

	ConstantBuffer<CB_VS_vertexshader_2d> cb_vs_vertexshader_2d;
	ConstantBuffer<CB_VS_vertexshader> cb_vs_vertexshader;
	ConstantBuffer<CB_VS_boneData> cb_vs_BoneInfo;
	ConstantBuffer<CB_PS_light> cb_ps_light;
	ConstantBuffer<CB_PS_Material> cb_ps_material;

	std::shared_ptr<Skybox> m_Skybox;
	std::shared_ptr<PixelShader> m_ShadowMapPshader;
	std::shared_ptr<VertexShader> m_PostProcesVshader;
	std::shared_ptr<PixelShader> m_PostProcesPshader;
	std::shared_ptr<Model> m_PostProcesWindowModel;

	int windowWidth = 0;
	int windowHeight = 0;
	const float m_BackgroundColor[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
	const float m_BlendFactors[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
	UINT m_DrawFlag = 0;

	ID3D11RenderTargetView * const m_NullRtv[DeviceResources::DeferredRenderChannelCount] = { NULL, };

	DirectX::XMMATRIX m_TargetViewProjectionMatrix;
	DirectX::BoundingFrustum m_CullFrustum;

	struct DrawFlag {
		enum {
			None = 0,
			Apply_MaterialVertexShader = 1,
			Apply_MaterialPixelShader = 1 << 1,
			Apply_SkinnedMeshBone = 1 << 2,
			Apply_MaterialTexture = 1 << 3,
			Apply_ViewFrustumCulling = 1 << 4,
			All = (1 << 31) - 1
		};
	};
};