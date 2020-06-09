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
class ComputeShader;
class Material;
class ShaderState;
class LightBase;

struct TextureBindTypes {
	enum {
		DeferredRenderingResource0 = 0,
		DeferredRenderingResource1,
		DeferredRenderingResource2,
		DeferredRenderingResource3,
		Depth,
		ShadowMap,
		SSAO,
		Bloom,
		Fur,
		Composition,

		MaterialAlbedo = 11,
		MaterialNormal,
		MaterialMetal,
		MaterialRoughness,
		MaterialEmission,

		Skybox = 21,
		IrradianceSkybox,

		Random = 31,
		Dithering,
		SpecularBRDF,
	};
};

struct DrawFlag {
	enum {
		None = 0,
		Apply_ObjectVertexShader =		1,
		Apply_ObjectPixelShader =		1 << 1,
		Apply_ObjectGeometryShader =	1 << 2,
		Apply_SkinnedMeshBone =			1 << 3,
		Apply_MaterialTexture =			1 << 4,
		Apply_ViewFrustumCulling =		1 << 5,
		All =							(1 << 30) - 1
	};
};

class Graphics : public DX11Resources {
public:
	bool Initialize(HWND hwnd, UINT width, UINT height);
	void RenderGizmo(const std::shared_ptr<RenderInfo>& renderInfo);
	void RenderSkybox();
	void RenderQuadPlane();
	void InitializeConstantBuffer();
	bool ProcessMaterialTable();
	bool ProcessShaderStateTable();
	void RenderBegin();
	void Pass_ShadowMap(const std::shared_ptr<LightBase> & light);
	void Pass_GBuffer();
	void Pass_SSAO();
	void Pass_Composition();
	void Pass_PostProcess();
	void Pass_Gizmo();
	void Pass_EditorUI();
	void Pass_Bloom(const UINT inout);
	void Pass_ToneMap(const UINT input, const UINT output);
	void Pass_GammaCorrection(const UINT input, const UINT output);
	void Pass_Blur(ID3D11ShaderResourceView** texIn, UINT width, UINT height);
	void Pass_UpSample(ID3D11ShaderResourceView ** texIn, ID3D11UnorderedAccessView ** texOut, UINT texInwidth, UINT texInheight);
	void Pass_DownSample(ID3D11ShaderResourceView** texIn, ID3D11UnorderedAccessView** texOut, UINT texInwidth, UINT texInheight);
	void RenderEnd();

	ConstantBuffer<GpuObjectBuffer> & GetCbVertexShader() { return m_GpuObjectBuffer; }
	ConstantBuffer<GpuBoneBuffer> & GetCbBoneInfo() { return m_GpuBoneBuffer; }
	ConstantBuffer<GpuAmbientLightBuffer> & GetCbAmbientLight() { return m_GpuAmbientLightBuffer; }
	ConstantBuffer<GpuSceneBuffer> & GetCbScene() { return m_GpuSceneBuffer; }
	ConstantBuffer<GpuFurBuffer> & GetCbFurData() { return m_GpuFurDataBuffer; }
	ConstantBuffer<GpuDownSampleBuffer> & GetCbDownSampleData() { return m_GpuDownSampleBuffer; }

	UINT GetWindowWidth() const { return m_WindowWidth; }
	UINT GetWindowHeight() const { return m_WindowHeight; }

private:
	void Render(const std::shared_ptr<RenderInfo>& renderer);
	void RenderMesh
	(
		const std::shared_ptr<Mesh>& mesh,
		const DirectX::XMMATRIX & worldMat,
		const DirectX::XMMATRIX & wvpMat
	);
	
	void ApplySceneBuffer();
	void ApplyMaterialProperties(const std::shared_ptr<Material>& material);
	void ApplyShaderState(const std::shared_ptr<ShaderState> & shaderState);
	void ApplySkinnedBone(const std::shared_ptr<RenderInfo>& renderInfo);

	bool IsInViewFrustum(const std::shared_ptr<RenderInfo>& renderInfo);

	DirectX::XMMATRIX m_TargetViewProjectionMatrix;
	DirectX::BoundingFrustum m_CullFrustum;

	ConstantBuffer<GpuObjectBuffer> m_GpuObjectBuffer;
	ConstantBuffer<GpuBoneBuffer> m_GpuBoneBuffer;
	ConstantBuffer<GpuSceneBuffer> m_GpuSceneBuffer;
	ConstantBuffer<GpuMaterialBuffer> m_GpuMaterialBuffer;
	ConstantBuffer<GpuBlurBuffer> m_GpuBlurBuffer;
	ConstantBuffer<GpuDownSampleBuffer> m_GpuDownSampleBuffer;
	ConstantBuffer<GpuSpotLightBuffer> m_GpuSpotLight;
	ConstantBuffer<GpuAmbientLightBuffer> m_GpuAmbientLightBuffer;
	ConstantBuffer<GpuFurBuffer> m_GpuFurDataBuffer;
	std::shared_ptr<Model> m_QuadWindowModel;
	std::shared_ptr<ComputeShader> m_BlurShader;
	std::shared_ptr<ComputeShader> m_UpSampleShader;
	std::shared_ptr<ComputeShader> m_DownSampleShader;

	std::weak_ptr<SharedMaterial> m_DefaultMaterial;
	std::weak_ptr<Texture> m_RandomTexture;
	std::weak_ptr<Texture> m_DitheringTexture;
	std::weak_ptr<Texture> m_IblBrdfTexture;
	std::weak_ptr<Texture> m_FurOpacityTexture;

	UINT m_WindowWidth = 0;
	UINT m_WindowHeight = 0;
	const float m_BackgroundColor[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
	const float m_BlendFactors[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
	UINT m_DrawFlag = 0;


	std::shared_ptr<Skybox> m_Skybox;
	std::shared_ptr<VertexShader> m_PostProcesVshader;
	std::shared_ptr<PixelShader> m_ShadowMapPshader;
	std::shared_ptr<PixelShader> m_SsaoShader;
	std::shared_ptr<PixelShader> m_CompositionShader;
	std::shared_ptr<PixelShader> m_BloomShader;
	std::shared_ptr<PixelShader> m_ToneMappingShader;
	std::shared_ptr<PixelShader> m_GammaCorrectionShader;
};