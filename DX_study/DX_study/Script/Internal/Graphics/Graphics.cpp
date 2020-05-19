#include "Graphics.h"

#include "../Engine/Ui.h"
#include "Model.h"
#include "Shaders.h"
#include "Skybox.h"
#include "BaseGeometry.h"
#include "AnimationClip.h"
#include "Material.h"
#include "DebugDraw.h"
#include "../../Util/Time.h"
#include "../../Util/Math.h"
#include "../Engine/Engine.h"
#include "../Engine/Ui.h"
#include "../Engine/Profiler.h"
#include "../Core/ObjectPool.h"
#include "../Core/GameObject.h"
#include "../Core/ImportHelper.h"
#include "../Core/Scene.h"
#include "../../Component/Transform.h"
#include "../../Component/RenderInfo.h"
#include "../../Component/Animator.h"
#include "../../Component/Light.h"
#include "../../GameObject/Camera.h"


using DirectX::operator*;
using DirectX::operator+;

bool Graphics::Initialize(HWND hwnd, UINT width, UINT height) {
	try {
		m_WindowWidth = width;
		m_WindowHeight = height;
		ThrowIfFailed(DX11Resources::Initialize(hwnd, width, height),		"Failed to initialize Device Resources");

		InitializeConstantBuffer();

		ThrowIfFailed(Importer::LoadBaseResources(),						"Failed to LoadBaseResources.");
		ThrowIfFailed(ProcessShaderStateTable(),							"Failed to ProcessMaterialTable.");
		ThrowIfFailed(ProcessMaterialTable(),								"Failed to ProcessMaterialTable.");
		ThrowIfFailed(BaseGeometry::Initialize(),							"Failed to Initialize BaseGeometry.");
		ThrowIfFailed(Importer::LoadModelResources(),						"Failed to LoadModelResources.");

		
		m_Skybox = std::make_shared<Skybox>();
		std::string filename[6] = { 
			"Data\\Skybox\\posx.jpg",	// +X
			"Data\\Skybox\\negx.jpg",	// -X
			"Data\\Skybox\\posy.jpg",	// +Y
			"Data\\Skybox\\negy.jpg",	// -Y
			"Data\\Skybox\\posz.jpg",	// +Z
			"Data\\Skybox\\negz.jpg"	// -Z
		};
		ThrowIfFailed(m_Skybox->Initialize(filename), "Failed to Initialize Skybox");
		m_GpuSceneBuffer.data.SkyBoxMaxMipLevel = m_Skybox->GetMaxMipLevel();

		GUI::InitImGUI(hwnd);

		m_QuadWindowModel =				Core::Find<Model>("Plane");

		m_PostProcesVshader =			Core::Find<VertexShader>("QuadPlane");
		m_ShadowMapPshader =			Core::Find<PixelShader>("ShadowMap");
		m_SsaoShader =					Core::Find<PixelShader>("SSAO");
		m_CompositionShader =			Core::Find<PixelShader>("Composition");
		m_BloomShader =					Core::Find<PixelShader>("Bloom");
		m_ToneMappingShader =			Core::Find<PixelShader>("ToneMapping");
		m_GammaCorrectionShader =		Core::Find<PixelShader>("GammaCorrection");
		m_BlurShader =					Core::Find<ComputeShader>("Blur");
		m_DownSampleShader =			Core::Find<ComputeShader>("ThresholdDownSample");

		m_RandomTexture =				Core::Find<Texture>("NoiseNormal");
		m_DitheringTexture =			Core::Find<Texture>("Dithering");
		m_IblBrdfTexture =				Core::Find<Texture>("ibl_brdf_lut");
		m_FurOpacityTexture =			Core::Find<Texture>("Fur_Noise");

		return true;
	}
	catch (CustomException & e) {
		StringHelper::ErrorLog(e);
		return false;
	}
}

void Graphics::InitializeConstantBuffer()
{
	ThrowIfFailed(m_GpuObjectBuffer.Initialize(), "Failed to Initialize CB_VS_vertexshader buffer.");
	ThrowIfFailed(m_GpuBoneBuffer.Initialize(), "Failed to Initialize cb_vs_BoneInfo buffer.");
	ThrowIfFailed(m_GpuMaterialBuffer.Initialize(), "Failed to Initialize cb_ps_material buffer.");
	ThrowIfFailed(m_GpuAmbientLightBuffer.Initialize(), "Failed to Initialize cb_ps_ambientLight buffer.");
	ThrowIfFailed(m_GpuSceneBuffer.Initialize(), "Failed to Initialize cb_ps_SceneBase buffer.");
	ThrowIfFailed(m_GpuSpotLight.Initialize(), "Failed to Initialize cb_ps_SpotLight buffer.");
	ThrowIfFailed(m_GpuFurDataBuffer.Initialize(), "Failed to Initialize cb_FurData buffer.");
	ThrowIfFailed(m_GpuBlurBuffer.Initialize(), "Failed to Initialize cb_cs_ThresholdBlur buffer.");
	ThrowIfFailed(m_GpuDownSampleBuffer.Initialize(), "Failed to Initialize cb_cs_DownSample buffer.");

	//BlurBuffer
	{
		m_GpuBlurBuffer.data.radius = GpuBlurBuffer::GAUSSIAN_RADIUS;

		float sigma = 10.f;
		float sigmaRcp = 1.f / sigma;
		float twoSigmaSq = 2 * sigma * sigma;

		float sum = 0.f;
		for (UINT i = 0; i <= GpuBlurBuffer::GAUSSIAN_RADIUS; ++i)
		{
			m_GpuBlurBuffer.data.coefficients[i] = (1.f / sigma) * std::expf(-static_cast<float>(i * i) / twoSigmaSq);
			sum += 2 * m_GpuBlurBuffer.data.coefficients[i];
		}
		sum -= m_GpuBlurBuffer.data.coefficients[0];

		float normalizationFactor = 1.f / sum;
		for (UINT i = 0; i <= GpuBlurBuffer::GAUSSIAN_RADIUS; ++i)
		{
			m_GpuBlurBuffer.data.coefficients[i] *= normalizationFactor;
		}

		m_GpuBlurBuffer.ApplyChanges();
		m_GpuDownSampleBuffer.ApplyChanges();
	}
}

bool Graphics::ProcessMaterialTable()
{
	try {
		auto table = Importer::LoadCSV("Data/CSV/MaterialTable.csv");
		size_t rowcount = table["Name"].size();
		for (int i = 0; i < rowcount; i++) {
			auto material = Core::CreateInstance<SharedMaterial>(table["Name"][i]);

			material->Albedo = Core::Find<Texture>(table["Albedo"][i]);
			material->Normal = Core::Find<Texture>(table["NormalMap"][i]);
			material->Metal = Core::Find<Texture>(table["MetalMap"][i]);
			material->Roughness = Core::Find<Texture>(table["RoughnessMap"][i]);
			material->Emission = Core::Find<Texture>(table["EmissionMap"][i]);
			material->NormalIntensity = std::stof(table["NormalIntensity"][i]);
			material->MetalIntensity = std::stof(table["MetalIntensity"][i]);
			material->RoughnessIntensity = std::stof(table["RoughnessIntensity"][i]);
			material->EmissionIntensity = std::stof(table["EmissionIntensity"][i]);
			
			auto splitted = Importer::SplitString(table["Color"][i], '/');
			material->Color.x = std::stof(splitted[0]) / 255;
			material->Color.y = std::stof(splitted[1]) / 255;
			material->Color.z = std::stof(splitted[2]) / 255;
			material->Color.w = std::stof(splitted[3]) / 255;
		}
		return true;
	}
	catch (CustomException &e) {
		StringHelper::ErrorLog(e);
		return false;
	}
}

bool Graphics::ProcessShaderStateTable() {
	try {
		auto table = Importer::LoadCSV("Data/CSV/ShaderStateTable.csv");
		size_t rowcount = table["Name"].size();
		for (int i = 0; i < rowcount; i++) {
			auto shaderState = Core::CreateInstance<ShaderState>(table["Name"][i]);

			shaderState->Vshader = Core::Find<VertexShader>(table["VertexShader"][i]);
			shaderState->Pshader = Core::Find<PixelShader>(table["PixelShader"][i]);
			shaderState->Gshader = Core::Find<GeometryShader>(table["GeometryShader"][i]);
		}
		return true;
	}
	catch (CustomException &e) {
		StringHelper::ErrorLog(e);
		return false;
	}
}

void Graphics::RenderBegin()
{
	ApplySceneBuffer();

	m_DeviceContext->ClearRenderTargetView(m_MainRenderTargetView.Get(), m_BackgroundColor);
	for (int i = 0; i < RenderTargetTypes::Max; i++) {
		m_DeviceContext->ClearRenderTargetView(m_RenderTargetViewArr[i].Get(), m_BackgroundColor);
	}
	m_DeviceContext->ClearDepthStencilView(m_MainDepthStencilView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	SetGSConstantBuffer(0, m_GpuObjectBuffer.GetAddressOf());
	SetGSConstantBuffer(1, m_GpuFurDataBuffer.GetAddressOf());

	SetPSConstantBuffer(0, m_GpuSceneBuffer.GetAddressOf());
	SetPSConstantBuffer(1, m_GpuSpotLight.GetAddressOf());
	SetPSConstantBuffer(2, m_GpuMaterialBuffer.GetAddressOf());
	SetPSConstantBuffer(3, m_GpuFurDataBuffer.GetAddressOf());

	SetVSConstantBuffer(0, m_GpuObjectBuffer.GetAddressOf());
	SetVSConstantBuffer(1, m_GpuBoneBuffer.GetAddressOf());

	SetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY::D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	SetDepthStencilState(m_DepthStencilState.Get());
	SetRasterizerState(m_RasterizerCullBack.Get());
	SetBlendState(m_BlendStateAlpha.Get(), m_BlendFactors);

	SetPsSampler(0, m_SamplerPointClamp.GetAddressOf());
	SetPsSampler(1, m_SamplerLinearClamp.GetAddressOf());
	SetPsSampler(2, m_SamplerLinearWrap.GetAddressOf());
	SetPsSampler(3, m_SamplerLinearMirror.GetAddressOf());
	SetPsSampler(4, m_SamplerAnisotropicWrap.GetAddressOf());
	SetPsSampler(5, m_SamplerTrilinearWrap.GetAddressOf());
}

void Graphics::Pass_GBuffer()
{
	Profiler::SampleBegin("Pass_GBuffer");

	auto mainCam = Engine::Get().GetCurrentScene().GetMainCam();
	m_TargetViewProjectionMatrix = mainCam->GetViewProjectionMatrix();
	m_CullFrustum = mainCam->GetViewFrustum();
	SetBlendState(m_BlendStateOpaque.Get(), m_BlendFactors);
	SetRenderTarget
	(
		DX11Resources::MAX_RENDER_TARGET_BINDING_COUNT,
		m_RenderTargetViewArr[0].GetAddressOf(),
		m_MainDepthStencilView.Get()
	);


	SetPSShaderResources(TextureBindTypes::Fur, 1, m_FurOpacityTexture.lock()->GetTextureResourceViewAddress());

	m_DrawFlag = DrawFlag::All;
	for (auto & renderInfo : Core::Pool<RenderInfo>::GetInstance().GetItems()) {
		Render(renderInfo);
	}

	RenderSkybox();

	SetRenderTarget
	(
		DX11Resources::MAX_RENDER_TARGET_BINDING_COUNT,
		m_NullRtv,
		NULL
	);
	SetVertexShader(NULL);
	SetPixelShader(NULL);
	SetGeometryShader(NULL);

	Profiler::SampleEnd("Pass_GBuffer");
}

void Graphics::Pass_SSAO()
{
	Profiler::SampleBegin("Pass_SSAO");

	SetPSShaderResources(TextureBindTypes::DeferredRenderingResource0, 1, m_RenderTargetSrvs[RenderTargetTypes::DeferredRenderingResource0].GetAddressOf());
	SetPSShaderResources(TextureBindTypes::DeferredRenderingResource1, 1, m_RenderTargetSrvs[RenderTargetTypes::DeferredRenderingResource1].GetAddressOf());
	SetPSShaderResources(TextureBindTypes::DeferredRenderingResource2, 1, m_RenderTargetSrvs[RenderTargetTypes::DeferredRenderingResource2].GetAddressOf());
	SetPSShaderResources(TextureBindTypes::DeferredRenderingResource3, 1, m_RenderTargetSrvs[RenderTargetTypes::DeferredRenderingResource3].GetAddressOf());
	SetPSShaderResources(TextureBindTypes::Depth, 1, m_MainDepthStencilSRV.GetAddressOf());

	{
		SetPSShaderResources(TextureBindTypes::SSAO, 1, m_NullSrv);
		SetRenderTarget
		(
			1,
			m_RenderTargetViewArr[RenderTargetTypes::SSAO].GetAddressOf(),
			NULL
		);

		SetPixelShader(m_SsaoShader->GetShader());

		RenderQuadPlane();

		SetRenderTarget
		(
			1,
			m_NullRtv,
			NULL
		);
	}

	Profiler::SampleEnd("Pass_SSAO");
}

void Graphics::Pass_Composition()
{
	auto l = Core::Find<GameObject>("Light")->GetComponent<SpotLight>();

	SetRenderTarget
	(
		1,
		m_RenderTargetViewArr[RenderTargetTypes::Composition0].GetAddressOf(),
		NULL
	);

	SetBlendState(m_BlendStateAlpha.Get(), m_BlendFactors);
	SetPixelShader(m_CompositionShader->GetShader());

	SetPSShaderResources(TextureBindTypes::DeferredRenderingResource0, 1, m_RenderTargetSrvs[RenderTargetTypes::DeferredRenderingResource0].GetAddressOf());
	SetPSShaderResources(TextureBindTypes::DeferredRenderingResource1, 1, m_RenderTargetSrvs[RenderTargetTypes::DeferredRenderingResource1].GetAddressOf());
	SetPSShaderResources(TextureBindTypes::DeferredRenderingResource2, 1, m_RenderTargetSrvs[RenderTargetTypes::DeferredRenderingResource2].GetAddressOf());
	SetPSShaderResources(TextureBindTypes::DeferredRenderingResource3, 1, m_RenderTargetSrvs[RenderTargetTypes::DeferredRenderingResource3].GetAddressOf());
	SetPSShaderResources(TextureBindTypes::Depth, 1, m_MainDepthStencilSRV.GetAddressOf());
	SetPSShaderResources(TextureBindTypes::ShadowMap, 1, l->GetShadowMapShaderResourceViewAddr());			
	SetPSShaderResources(TextureBindTypes::SSAO, 1, m_RenderTargetSrvs[RenderTargetTypes::SSAO].GetAddressOf());

	SetPSShaderResources(TextureBindTypes::Random, 1, m_RandomTexture.lock()->GetTextureResourceViewAddress());
	SetPSShaderResources(TextureBindTypes::Dithering, 1, m_DitheringTexture.lock()->GetTextureResourceViewAddress());	
	SetPSShaderResources(TextureBindTypes::SpecularBRDF, 1, m_IblBrdfTexture.lock()->GetTextureResourceViewAddress());	
	SetPSShaderResources(TextureBindTypes::Skybox, 1, m_Skybox->GetCubeMapView());	
	SetPSShaderResources(TextureBindTypes::IrradianceSkybox, 1, m_Skybox->GetIrMapView());	
	
	RenderQuadPlane();

	SetPSShaderResources(0, 10, m_NullSrv);
	SetPSShaderResources(9, 1, m_NullSrv);
	SetRenderTarget
	(
		DX11Resources::MAX_RENDER_TARGET_BINDING_COUNT,
		m_NullRtv,
		NULL
	);
}

void Graphics::Render(const std::shared_ptr<RenderInfo>& renderInfo)
{
	auto & renderables = renderInfo->GetRenerables();
	renderInfo->m_IsVisible = false;

	if (renderables.size() == 0) {
		return;
	}

	bool isVisible = IsInViewFrustum(renderInfo);
	renderInfo->m_IsVisible = isVisible;
	if (!isVisible) {
		return;
	}

	ApplySkinnedBone(renderInfo);

	auto& tf = renderInfo->m_GameObject->GetTransform();
	auto& worldMat = tf.GetWorldMatrix();
	auto wvpMat = worldMat * m_TargetViewProjectionMatrix;
	
	for (auto & r : renderables) {
		ApplyMaterialProperties(r.GetMaterial());
		ApplyShaderState(r.GetShaderState());
		RenderMesh(r.GetMesh(), worldMat, wvpMat);
	}
}

void Graphics::RenderMesh(
	const std::shared_ptr<Mesh>& mesh,
	const DirectX::XMMATRIX & worldMat, 
	const DirectX::XMMATRIX & wvpMat)
{
	m_GpuObjectBuffer.data.wvpMatrix = mesh->GetWorldMatrix() * wvpMat; 
	m_GpuObjectBuffer.data.worldMatrix = mesh->GetWorldMatrix() * worldMat; 
	m_GpuObjectBuffer.ApplyChanges();

	SetVertexBuffer(mesh->GetVertexBufferAddr(), mesh->GetVertexBufferStridePtr());
	SetIndexBuffer(mesh->GetIndexBuffer().Get());

	DrawIndexed(mesh->GetIndexBuffer().IndexCount());
}

void Graphics::RenderSkybox()
{
	SetVSInputLayout(m_Skybox->GetVertexShader()->GetInputLayout());
	SetVertexShader(m_Skybox->GetVertexShader()->GetShader());
	SetGeometryShader(NULL);
	SetPixelShader(m_Skybox->GetPixelShader()->GetShader());
	SetPSShaderResources(TextureBindTypes::Skybox, 1, m_Skybox->GetCubeMapView());
	SetPSShaderResources(TextureBindTypes::IrradianceSkybox, 1, m_Skybox->GetIrMapView());

	SetRasterizerState(m_RasterizerCullNone.Get());

	m_DrawFlag = DrawFlag::None;

	auto mainCam = Engine::Get().GetCurrentScene().GetMainCam();
	auto worldMat = DirectX::XMMatrixTranslationFromVector(mainCam->GetTransform().positionVec);
	auto wvpMat = worldMat * mainCam->GetViewProjectionMatrix();
	for (auto & r : m_Skybox->GetModel()->GetDefaultRenderables()) {
		RenderMesh(r.GetMesh(), worldMat, wvpMat);
	}

	SetRasterizerState(m_RasterizerCullBack.Get());
}

void Graphics::RenderQuadPlane()
{
	auto& mesh = m_QuadWindowModel->GetDefaultRenderables()[0].GetMesh();
	SetVertexShader(m_PostProcesVshader->GetShader());
	SetVSInputLayout(m_PostProcesVshader->GetInputLayout());
	SetVertexBuffer(mesh->GetVertexBufferAddr(), mesh->GetVertexBufferStridePtr());
	SetIndexBuffer(mesh->GetIndexBuffer().Get());
	DrawIndexed(mesh->GetIndexBuffer().IndexCount());
}

void Graphics::RenderGizmo(const std::shared_ptr<RenderInfo>& renderer)
{
	if (renderer->m_Model) {
		for (auto& r : renderer->GetRenerables()) {
			GUI::Draw(m_PrimitiveBatch.get(), Math::GetGlobalBoundingBox(r.GetMesh()->GetLocalAABB(), renderer->GetGameObject()->GetTransform()));
		}
	}
}

void Graphics::ApplySceneBuffer()
{
	static auto mainCam = Core::GetCurrentScene().GetMainCam();
	static auto light = Core::Find<GameObject>("Light");
	static auto lightc = light->GetComponent<SpotLight>();

	DirectX::XMVECTOR spotLightPlaneCenter = light->GetTransform().positionVec + light->GetTransform().GetForwardVector() * lightc->GetRange();

	DirectX::XMStoreFloat3(&m_GpuSpotLight.data.forwardVector, light->GetTransform().GetForwardVector());
	m_GpuSpotLight.data.color = lightc->Color;
	m_GpuSpotLight.data.strength = lightc->Strength;
	m_GpuSpotLight.data.position = light->GetTransform().position;
	m_GpuSpotLight.data.attenuation = lightc->Attentuation;
	m_GpuSpotLight.data.spotAngle = lightc->GetSpotAngle() * 0.5f * Math::Deg2Rad;
	m_GpuSpotLight.data.range = lightc->GetRange();
	m_GpuSpotLight.data.vpMat = lightc->GetLightViewProjectMat();
	m_GpuSpotLight.data.conePlaneD = DirectX::XMVector3Dot(spotLightPlaneCenter, light->GetTransform().GetForwardVector()).m128_f32[0] * -1;
	m_GpuSpotLight.ApplyChanges();

	DirectX::XMStoreFloat3(&m_GpuSceneBuffer.data.CamPosition, mainCam->GetTransform().positionVec);
	m_GpuSceneBuffer.data.CamViewRange = mainCam->GetViewRange();
	m_GpuSceneBuffer.data.SkyBoxMaxMipLevel = m_Skybox->GetMaxMipLevel();
	m_GpuSceneBuffer.data.CameraForward = mainCam->GetTransform().GetForwardVector();
	m_GpuSceneBuffer.data.ElapsedTime = Time::GetTime();
	m_GpuSceneBuffer.data.DeltaTime = Time::GetDeltaTime();
	m_GpuSceneBuffer.data.InverseViewMat = DirectX::XMMatrixInverse(nullptr, mainCam->GetViewMatrix());
	m_GpuSceneBuffer.data.InverseProjMat = DirectX::XMMatrixInverse(nullptr, mainCam->GetProjectionMatrix());
	m_GpuSceneBuffer.ApplyChanges();

	m_GpuFurDataBuffer.ApplyChanges();
}

void Graphics::ApplyMaterialProperties(const std::shared_ptr<Material>& material)
{
	if (m_DrawFlag & DrawFlag::Apply_MaterialTexture){
		SetPSShaderResources(TextureBindTypes::MaterialAlbedo, 1, material->Albedo ?
			material->Albedo->GetTextureResourceViewAddress() :
			Texture::GetDefault()->GetTextureResourceViewAddress());
		SetPSShaderResources(TextureBindTypes::MaterialNormal, 1, material->Normal ?
			material->Normal->GetTextureResourceViewAddress() :
			Texture::GetDefaultNormalMap()->GetTextureResourceViewAddress());
		SetPSShaderResources(TextureBindTypes::MaterialMetal, 1, material->Metal ?
			material->Metal->GetTextureResourceViewAddress() :
			Texture::GetDefault()->GetTextureResourceViewAddress());
		SetPSShaderResources(TextureBindTypes::MaterialRoughness, 1, material->Roughness ?
			material->Roughness->GetTextureResourceViewAddress() :
			Texture::GetDefault()->GetTextureResourceViewAddress());
		SetPSShaderResources(TextureBindTypes::MaterialEmission, 1, material->Emission ?
			material->Emission->GetTextureResourceViewAddress() :
			Texture::GetDefault()->GetTextureResourceViewAddress());
	}

	m_GpuMaterialBuffer.data.color = material->Color;
	m_GpuMaterialBuffer.data.NormalIntensity = material->NormalIntensity;
	m_GpuMaterialBuffer.data.RoughnessIntensity = material->RoughnessIntensity;
	m_GpuMaterialBuffer.data.EmissionIntensity = material->EmissionIntensity;
	m_GpuMaterialBuffer.data.MetalIntensity = material->MetalIntensity;
	m_GpuMaterialBuffer.ApplyChanges();
}

void Graphics::ApplyShaderState(const std::shared_ptr<ShaderState>& shaderState)
{
	if (m_DrawFlag & DrawFlag::Apply_ObjectVertexShader) {
		auto vs = shaderState->Vshader ? shaderState->Vshader : VertexShader::GetDefault();
		SetVSInputLayout(vs->GetInputLayout());
		SetVertexShader(vs->GetShader());
	}
	if (m_DrawFlag & DrawFlag::Apply_ObjectPixelShader) {
		auto ps = shaderState->Pshader ? shaderState->Pshader : PixelShader::GetDefault();
		SetPixelShader(ps->GetShader());
	}
	if (m_DrawFlag & DrawFlag::Apply_ObjectGeometryShader) {
		auto shader = shaderState->Gshader ? shaderState->Gshader->GetShader() : NULL;
		SetGeometryShader(shader);
	}
	else {
		auto shader = shaderState->Gshader ? GeometryShader::GetDefault()->GetShader() : NULL;
		SetGeometryShader(shader);
	}
}

void Graphics::ApplySkinnedBone(const std::shared_ptr<RenderInfo>& renderer)
{
	if (m_DrawFlag & DrawFlag::Apply_SkinnedMeshBone &&
		renderer->Anim &&
		renderer->Anim->GetClip())
	{
		CopyMemory(m_GpuBoneBuffer.data.boneTransform,
			renderer->Anim->GetAnimResult().data(),
			renderer->Anim->GetAnimResult().size() * sizeof(DirectX::XMMATRIX));
		m_GpuBoneBuffer.ApplyChanges();
	}
}

bool Graphics::IsInViewFrustum(const std::shared_ptr<RenderInfo>& renderer)
{
	if ((m_DrawFlag & DrawFlag::Apply_ViewFrustumCulling) == false) {
		return true;
	}

	bool isVisible = false;

	auto& tf = renderer->m_GameObject->GetTransform();
	for (auto & r : renderer->GetRenerables()) {
		auto globalAABB = Math::GetGlobalBoundingBox(r.GetMesh()->GetLocalAABB(), tf);
		auto containment = m_CullFrustum.Contains(globalAABB);
		isVisible |= containment != DirectX::DISJOINT;
	}

	return isVisible;
}

void Graphics::Pass_PostProcess()
{
	Profiler::SampleBegin("Pass_PostProcess");

	auto inType = RenderTargetTypes::Composition0;
	auto outType = RenderTargetTypes::Composition1;

	Pass_Bloom(inType);

	Pass_ToneMap(inType, outType);
	std::swap(inType, outType);

	Pass_GammaCorrection(inType, outType);
	std::swap(inType, outType);

	Profiler::SampleEnd("Pass_PostProcess");
}

void Graphics::Pass_Bloom(const UINT inout)
{
	static const UINT MAX_DOWNSAMPLE_COUNT = 3;

	ID3D11ShaderResourceView** downSampleIn = m_RenderTargetSrvs[inout].GetAddressOf();
	ID3D11UnorderedAccessView** downSampleOut = nullptr;
	UINT width = m_WindowWidth;
	UINT height = m_WindowHeight;

	for (UINT i = 0; i < MAX_DOWNSAMPLE_COUNT; i++) {
		downSampleOut = m_RenderTargetUavs[RenderTargetTypes::HalfSize + i].GetAddressOf();
		Pass_DownSample(downSampleIn, downSampleOut, width, height);

		width *= 0.5f;
		height *= 0.5f;
		downSampleIn = m_RenderTargetSrvs[RenderTargetTypes::HalfSize + i].GetAddressOf();
	}

	SetBlendState(m_BlendStateAdditive.Get(), m_BackgroundColor);
	SetPSShaderResources(TextureBindTypes::Bloom, 1, m_RenderTargetSrvs[RenderTargetTypes::HalfQuarterSize].GetAddressOf());
	SetRenderTarget
	(
		1,
		m_RenderTargetViewArr[inout].GetAddressOf(),
		NULL
	);

	SetPixelShader(m_BloomShader->GetShader());
	RenderQuadPlane();

	SetPSShaderResources(0, DX11Resources::MAX_RENDER_TARGET_BINDING_COUNT + 1, m_NullSrv);
	SetPSShaderResources(TextureBindTypes::Bloom, 1, m_NullSrv);
	SetRenderTarget
	(
		DX11Resources::MAX_RENDER_TARGET_BINDING_COUNT,
		m_NullRtv,
		NULL
	);
	SetBlendState(m_BlendStateOpaque.Get(), m_BackgroundColor);

}

void Graphics::Pass_ToneMap(const UINT input, const UINT output)
{
	SetPSShaderResources(TextureBindTypes::Composition, 1, m_RenderTargetSrvs[input].GetAddressOf());
	SetRenderTarget
	(
		1,
		m_RenderTargetViewArr[output].GetAddressOf(),
		m_MainDepthStencilView.Get()
	);

	SetPixelShader(m_ToneMappingShader->GetShader());
	RenderQuadPlane();

	SetPSShaderResources(TextureBindTypes::Composition, 1, m_NullSrv);
	SetRenderTarget
	(
		DX11Resources::MAX_RENDER_TARGET_BINDING_COUNT,
		m_NullRtv,
		NULL
	);
}

void Graphics::Pass_GammaCorrection(const UINT input, const UINT output)
{
	SetPSShaderResources(TextureBindTypes::Composition, 1, m_RenderTargetSrvs[input].GetAddressOf());
	SetRenderTarget
	(
		1,
		m_RenderTargetViewArr[output].GetAddressOf(),
		m_MainDepthStencilView.Get()
	);

	SetPixelShader(m_GammaCorrectionShader->GetShader());
	RenderQuadPlane();

	SetPSShaderResources(TextureBindTypes::Composition, 1, m_NullSrv);
	SetRenderTarget
	(
		DX11Resources::MAX_RENDER_TARGET_BINDING_COUNT,
		m_NullRtv,
		NULL
	);
}

void Graphics::Pass_ShadowMap(const std::shared_ptr<LightBase> & light)
{
	Profiler::SampleBegin("Pass_ShadowMap");

	auto mainCam = Engine::Get().GetCurrentScene().GetMainCam();
	auto spotLight = std::dynamic_pointer_cast<SpotLight>(light);
	auto& lightTransform = spotLight->GetGameObject()->GetTransform();

	m_TargetViewProjectionMatrix = spotLight->GetLightViewProjectMat();
	m_CullFrustum = DirectX::BoundingFrustum(spotLight->GetProjectionMatrix());
	m_CullFrustum.Origin = lightTransform.position;
	DirectX::XMStoreFloat4(&m_CullFrustum.Orientation, lightTransform.GetQuaternion());

	m_DeviceContext->ClearDepthStencilView(m_SubDepthStencilView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	m_DeviceContext->ClearRenderTargetView(spotLight->GetShadowMapRenderTargetView(), m_BackgroundColor);
	SetRenderTarget
	(
		DX11Resources::MAX_RENDER_TARGET_BINDING_COUNT,
		m_NullRtv,
		NULL
	);

	SetRenderTarget(1, spotLight->GetShadowMapRenderTargetViewAddr(), m_SubDepthStencilView.Get());
	SetPixelShader(m_ShadowMapPshader->GetShader());

	m_DrawFlag =
		DrawFlag::Apply_ObjectVertexShader |
		DrawFlag::Apply_SkinnedMeshBone;

	for (auto & r : Core::Pool<RenderInfo>::GetInstance().GetItems()) {
		Render(r);
	}

	SetVertexShader(NULL);
	SetPixelShader(NULL);
	SetGeometryShader(NULL);
	SetRenderTarget
	(
		1,
		m_NullRtv,
		NULL
	);

	Profiler::SampleEnd("Pass_ShadowMap");
}

void Graphics::Pass_EditorUI()
{
	Profiler::SampleBegin("Pass_EditorUI");

	auto light = Core::Find<GameObject>("Light")->GetComponent<SpotLight>();
	
	SetRenderTarget
	(
		1, 
		m_MainRenderTargetView.GetAddressOf(), 
		NULL
	);

	GUI::NewFrame();
	
	GUI::DrawEditorUI(m_RenderTargetSrvs[DX11Resources::MAX_RENDER_TARGET_BINDING_COUNT].Get());
	
	GUI::Render();

	SetRenderTarget
	(
		1,
		m_NullRtv,
		NULL
	);
	Profiler::SampleEnd("Pass_EditorUI");

}

void Graphics::Pass_Gizmo()
{
	Profiler::SampleBegin("Pass_Gizmo");

	auto& camera = *Engine::Get().GetCurrentScene().GetMainCam();

	m_BasicEffect->SetVertexColorEnabled(true);
	m_BasicEffect->SetView(camera.GetViewMatrix());
	m_BasicEffect->SetProjection(camera.GetProjectionMatrix());

	SetBlendState(m_BlendStateOpaque.Get(), nullptr);
	SetDepthStencilState(m_CommonState->DepthNone());
	SetRasterizerState(m_RasterizerCullNone.Get());

	m_BasicEffect->Apply(m_DeviceContext.Get());

	SetVSInputLayout(m_DebugInputLayout.Get());

	SetRenderTarget
	(
		1,
		m_RenderTargetViewArr[DX11Resources::MAX_RENDER_TARGET_BINDING_COUNT].GetAddressOf(),
		m_MainDepthStencilView.Get()
	);

	m_PrimitiveBatch->Begin();

	for (auto & r : Core::Pool<RenderInfo>::GetInstance().GetItems()) {
		RenderGizmo(r);
	}

	m_PrimitiveBatch->End();

	SetRenderTarget
	(
		DX11Resources::MAX_RENDER_TARGET_BINDING_COUNT,
		m_NullRtv,
		NULL
	);
	Profiler::SampleEnd("Pass_Gizmo");

}

void Graphics::Pass_Blur(ID3D11ShaderResourceView** texIn, UINT width, UINT height)
{
	//ThresHold DownSample
	{
		m_GpuDownSampleBuffer.ApplyChanges();
		SetComputeShader(m_DownSampleShader->GetShader());
		SetCSConstantBuffer(0, m_GpuDownSampleBuffer.GetAddressOf());
		SetCSShaderResources(0, 1, texIn);
		SetCSUavResources(0, 1, m_RenderTargetUavs[RenderTargetTypes::BlurOut].GetAddressOf());

		DispatchComputeShader(m_WindowWidth / 16, m_WindowHeight / 16);

		SetCSUavResources(0, 1, m_NullUav);
		SetCSShaderResources(0, 1, m_NullSrv);
	}
	//Blur
	{
		
		ID3D11ShaderResourceView* csSRVs[2] = { 
			m_RenderTargetSrvs[RenderTargetTypes::BlurOut].Get(),
			m_RenderTargetSrvs[RenderTargetTypes::BlurIn].Get() };
		ID3D11UnorderedAccessView* csUAVs[2] = {
			m_RenderTargetUavs[RenderTargetTypes::BlurIn].Get(),
			m_RenderTargetUavs[RenderTargetTypes::BlurOut].Get() };

		SetComputeShader(m_BlurShader->GetShader());

		for (UINT i = 0; i < 2; ++i)
		{
			m_GpuBlurBuffer.data.direction = i;
			m_GpuBlurBuffer.ApplyChanges();

			SetCSConstantBuffer(0, m_GpuBlurBuffer.GetAddressOf());
			SetCSShaderResources(0, 1, &csSRVs[i]);
			SetCSUavResources(0, 1, &csUAVs[i]);

			DispatchComputeShader(width / 16, height / 16);

			SetCSShaderResources(0, 1, m_NullSrv);
			SetCSUavResources(0, 1, m_NullUav);
		}
	}
}

void Graphics::Pass_DownSample(
	ID3D11ShaderResourceView ** texIn, 
	ID3D11UnorderedAccessView ** texOut, 
	UINT texInwidth, 
	UINT texInheight)
{
	m_GpuDownSampleBuffer.ApplyChanges();
	SetComputeShader(m_DownSampleShader->GetShader());
	SetCSConstantBuffer(0, m_GpuDownSampleBuffer.GetAddressOf());
	SetCSShaderResources(0, 1, texIn);
	SetCSUavResources(0, 1, texOut);

	DispatchComputeShader(texInwidth / 8, texInheight / 8);

	SetCSUavResources(0, 1, m_NullUav);
	SetCSShaderResources(0, 1, m_NullSrv);
}

void Graphics::RenderEnd()
{
	Profiler::SampleBegin("Present Swapchain");

	m_Swapchain->Present(1, NULL);

	Profiler::SampleEnd("Present Swapchain");
}