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

	ConstantBuffer<GpuObjectBuffer> & GetCbVertexShader() { return m_GpuObjectBuffer; }
	ConstantBuffer<GpuBoneBuffer> & GetCbBoneInfo() { return m_GpuBoneBuffer; }
	ConstantBuffer<GpuAmbientLightBuffer> & GetCbAmbientLight() { return m_GpuAmbientLightBuffer; }
	ConstantBuffer<GpuSceneBuffer> & GetCbScene() { return m_GpuSceneBuffer; }
	ConstantBuffer<GpuFurBuffer> & GetCbFurData() { return m_GpuFurDataBuffer; }

	UINT GetWindowWidth() const { return m_WindowWidth; }
	UINT GetWindowHeight() const { return m_WindowHeight; }

private:
	void ApplyMaterialProperties(const std::shared_ptr<Material>& material);
	void ApplySkinnedBone(const std::shared_ptr<RenderInfo>& renderer);
	bool ViewFrustumCull(const std::shared_ptr<RenderInfo>& renderer);

	ConstantBuffer<GpuObjectBuffer> m_GpuObjectBuffer;
	ConstantBuffer<GpuBoneBuffer> m_GpuBoneBuffer;
	ConstantBuffer<GpuSceneBuffer> m_GpuSceneBuffer;
	ConstantBuffer<GpuMaterialBuffer> m_GpuMaterialBuffer;
	ConstantBuffer<GpuBlurBuffer> m_GpuBlurBuffer;
	ConstantBuffer<GpuDownSampleBuffer> m_GpuDownSampleBuffer;
	ConstantBuffer<GpuSpotLightBuffer> m_GpuSpotLight;
	ConstantBuffer<GpuAmbientLightBuffer> m_GpuAmbientLightBuffer;
	ConstantBuffer<GpuFurBuffer> m_GpuFurDataBuffer;
	
	
	std::shared_ptr<Skybox> m_Skybox;
	std::shared_ptr<PixelShader> m_ShadowMapPshader;
	std::shared_ptr<VertexShader> m_PostProcesVshader;
	std::shared_ptr<PixelShader> m_PostProcesPshader;
	std::shared_ptr<Model> m_QuadWindowModel;

	UINT m_WindowWidth = 0;
	UINT m_WindowHeight = 0;
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