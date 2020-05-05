#pragma once
#include <vector>
#include <memory>
#include <unordered_map>
#include "ConstantBuffer.h"
#include "../Engine/DX11Resources.h"
#include "../../Util/Math.h"

class BufferBase;
class Texture;
class SharedMaterial;
class Model;
class SkinnedModel;
class Mesh;
class Mesh;
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

class Graphics : public DX11Resources {
public:
	bool Initialize(HWND hwnd, UINT width, UINT height);
	bool ProcessMaterialTable();

	void RenderBegin();
	void RenderModels();
	void Render(const std::shared_ptr<RenderInfo>& renderer);
	void DrawMesh(const std::shared_ptr<Mesh>& mesh,
		const DirectX::XMMATRIX & worldMat, 
		const DirectX::XMMATRIX & wvpMat);
	void DebugDraw(const std::shared_ptr<RenderInfo>& renderer);
	
	void PostProcess();
	void DrawGui();
	void DrawShadowMap(const std::shared_ptr<LightBase> & light);
	void DrawSkybox();
	void DrawGuiDebug();
	void ComputeShdaderTest();
	void RenderEnd();

	ConstantBuffer<CB_VS_vertexshader_2d> & GetCbVertexShader2D() { return cb_vs_vertexshader_2d; }
	ConstantBuffer<CB_VS_vertexshader> & GetCbVertexShader() { return cb_vs_vertexshader; }
	ConstantBuffer<CB_VS_boneData> & GetCbBoneInfo() { return cb_vs_BoneInfo; }
	ConstantBuffer<CB_PS_AmbientLight> & GetCbAmbientLight() { return cb_ps_ambientLight; }
	ConstantBuffer<CB_PS_Scene> & GetCbScene() { return cb_ps_SceneBase; }
	ConstantBuffer<CB_FurShader> & GetCbFurData() { return cb_FurData; }

	UINT GetWindowWidth() const { return windowWidth; }
	UINT GetWindowHeight() const { return windowHeight; }

private:
	void ApplyMaterialProperties(const std::shared_ptr<Material>& material);
	void ApplySkinnedBone(const std::shared_ptr<RenderInfo>& renderer);
	bool ViewFrustumCull(const std::shared_ptr<RenderInfo>& renderer);

	ConstantBuffer<CB_VS_vertexshader_2d> cb_vs_vertexshader_2d;
	ConstantBuffer<CB_VS_vertexshader> cb_vs_vertexshader;
	ConstantBuffer<CB_VS_boneData> cb_vs_BoneInfo;

	ConstantBuffer<CB_PS_Scene> cb_ps_SceneBase;
	ConstantBuffer<CB_PS_SpottLight> cb_ps_SpotLight;
	ConstantBuffer<CB_PS_Material> cb_ps_material;
	ConstantBuffer<CB_PS_AmbientLight> cb_ps_ambientLight;
	ConstantBuffer<CB_FurShader> cb_FurData;
	
	ConstantBuffer<CB_CS_ThresholdBlur> cb_cs_ThresholdBlur;
	ConstantBuffer<CB_CS_DownSample> cb_cs_DownSample;
	
	std::shared_ptr<Skybox> m_Skybox;
	std::shared_ptr<PixelShader> m_ShadowMapPshader;
	std::shared_ptr<VertexShader> m_PostProcesVshader;
	std::shared_ptr<PixelShader> m_PostProcesPshader;
	std::shared_ptr<Model> m_PostProcesWindowModel;

	UINT windowWidth = 0;
	UINT windowHeight = 0;
	const float m_BackgroundColor[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
	const float m_BlendFactors[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
	UINT m_DrawFlag = 0;

	

	DirectX::XMMATRIX m_TargetViewProjectionMatrix;
	DirectX::BoundingFrustum m_CullFrustum;

	struct DrawFlag {
		enum {
			None = 0,
			Apply_MaterialVertexShader = 1,
			Apply_MaterialPixelShader = 1 << 1,
			Apply_MaterialGeometryShader = 1 << 2,
			Apply_SkinnedMeshBone = 1 << 3,
			Apply_MaterialTexture = 1 << 4,
			Apply_ViewFrustumCulling = 1 << 5,
			All = (1 << 30) - 1
		};
	};

	std::weak_ptr<SharedMaterial> m_DefaultMaterial;
	std::weak_ptr<Texture> m_RandomTexture;
	std::weak_ptr<Texture> m_DitheringTexture;
	std::weak_ptr<Texture> m_IblBrdfTexture;
	std::weak_ptr<Texture> m_FurOpacityTexture;

	
};