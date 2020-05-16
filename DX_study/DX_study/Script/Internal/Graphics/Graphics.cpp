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
#include "../Core/ObjectPool.h"
#include "../Core/GameObject.h"
#include "../Core/ImportHelper.h"
#include "../Core/Scene.h"
#include "../../Component/Transform.h"
#include "../../Component/Renderable.h"
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

		ThrowIfFailed(Importer::LoadBaseResources(),							"Failed to LoadBaseResources.");
		ThrowIfFailed(ProcessMaterialTable(),									"Failed to ProcessMaterialTable.");
		ThrowIfFailed(BaseGeometry::Initialize(),								"Failed to Initialize BaseGeometry.");
		ThrowIfFailed(Importer::LoadModelResources(),							"Failed to LoadModelResources.");
		
		m_Skybox = std::make_shared<Skybox>();
		std::string filename[6] = { //순서는 나중에
			"Data\\Skybox\\Test1\\oasisnight_ft_p.png",	// +X
			"Data\\Skybox\\Test1\\oasisnight_bk_p.png",	// -X
			"Data\\Skybox\\Test1\\oasisnight_up_p.png",	// +Y
			"Data\\Skybox\\Test1\\oasisnight_dn_p.png",	// -Y
			"Data\\Skybox\\Test1\\oasisnight_rt_p.png",	// +Z
			"Data\\Skybox\\Test1\\oasisnight_lf_p.png"	// -Z
		};
		ThrowIfFailed(m_Skybox->Initialize(filename), "Failed to Initialize Skybox");
		m_GpuSceneBuffer.data.SkyBoxMaxMipLevel = m_Skybox->GetMaxMipLevel();

		GUI::InitImGUI(hwnd);

		m_QuadWindowModel = Core::Find<Model>("Plane");
		m_PostProcesVshader = Core::Find<VertexShader>("QuadPlane");
		m_ShadowMapPshader = Core::Find<PixelShader>("ShadowMap");
		m_SsaoShader = Core::Find<PixelShader>("SSAO");
		m_CompositionShader = Core::Find<PixelShader>("Composition");
		m_LightShader = Core::Find<PixelShader>("Light");
		m_BloomShader = Core::Find<PixelShader>("Bloom");
		m_ToneMappingShader = Core::Find<PixelShader>("ToneMapping");
		m_GammaCorrectionShader = Core::Find<PixelShader>("GammaCorrection");

		m_RandomTexture = Core::Find<Texture>("NoiseNormal");
		m_DitheringTexture = Core::Find<Texture>("Dithering");
		m_IblBrdfTexture = Core::Find<Texture>("ibl_brdf_lut");
		m_FurOpacityTexture = Core::Find<Texture>("Fur_Noise");
		m_BlurShader = Core::Find<ComputeShader>("Blur");
		m_DownSampleShader = Core::Find<ComputeShader>("ThresholdDownSample");

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

			material->Vshader = Core::Find<VertexShader>(table["VertexShader"][i]);
			material->Pshader = Core::Find<PixelShader>(table["PixelShader"][i]);
			material->Gshader = Core::Find<GeometryShader>(table["GeometryShader"][i]);
			material->Albedo = Core::Find<Texture>(table["Albedo"][i]);
			material->Normal = Core::Find<Texture>(table["NormalMap"][i]);
			material->Metal = Core::Find<Texture>(table["MetalMap"][i]);
			material->Roughness = Core::Find<Texture>(table["RoughnessMap"][i]);
			material->Specular = Core::Find<Texture>(table["SpecularMap"][i]);
			material->NormalIntensity = std::stof(table["NormalIntensity"][i]);
			material->MetalIntensity = std::stof(table["MetalIntensity"][i]);
			material->RoughnessIntensity = std::stof(table["RoughnessIntensity"][i]);
			material->SpecularIntensity = std::stof(table["SpecularIntensity"][i]);
			
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
	auto mainCam = Engine::Get().GetCurrentScene().GetMainCam();
	m_TargetViewProjectionMatrix = mainCam->GetViewProjectionMatrix();
	m_CullFrustum = mainCam->GetViewFrustum();

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
}

void Graphics::Pass_Light()
{
	auto l = Core::Find<GameObject>("Light")->GetComponent<SpotLight>();
	SetPSShaderResources(TextureBindTypes::Position, 1, m_RenderTargetSrvs[RenderTargetTypes::Position].GetAddressOf());
	SetPSShaderResources(TextureBindTypes::Normal, 1, m_RenderTargetSrvs[RenderTargetTypes::Normal].GetAddressOf());
	SetPSShaderResources(TextureBindTypes::Albedo, 1, m_RenderTargetSrvs[RenderTargetTypes::Albedo].GetAddressOf());
	SetPSShaderResources(TextureBindTypes::Material, 1, m_RenderTargetSrvs[RenderTargetTypes::Material].GetAddressOf());
	SetPSShaderResources(TextureBindTypes::Depth, 1, m_MainDepthStencilSRV.GetAddressOf());
	//SetPSShaderResources(TextureBindTypes::Depth, 1, m_RenderTargetSrvs[RenderTargetTypes::Depth].GetAddressOf());
	SetPSShaderResources(TextureBindTypes::ShadowMap, 1, l->GetShadowMapShaderResourceViewAddr());			//shadowmap
	SetPSShaderResources(TextureBindTypes::Dithering, 1, m_DitheringTexture.lock()->GetTextureResourceViewAddress());	//Dithering

	SetBlendState(m_BlendStateOpaque.Get(), m_BackgroundColor);

	{
		SetRenderTarget
		(
			1,
			m_RenderTargetViewArr[RenderTargetTypes::Light].GetAddressOf(),
			NULL
		);

		SetPixelShader(m_LightShader->GetShader());

		RenderQuadPlane();

		SetRenderTarget
		(
			1,
			m_NullRtv,
			NULL
		);
	}
	SetBlendState(m_BlendStateAlpha.Get(), m_BackgroundColor);
	
}

void Graphics::Pass_SSAO()
{
	SetPSShaderResources(TextureBindTypes::Position, 1, m_RenderTargetSrvs[RenderTargetTypes::Position].GetAddressOf());
	SetPSShaderResources(TextureBindTypes::Normal, 1, m_RenderTargetSrvs[RenderTargetTypes::Normal].GetAddressOf());
	SetPSShaderResources(TextureBindTypes::Depth, 1, m_RenderTargetSrvs[RenderTargetTypes::Depth].GetAddressOf());

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

	//Pass_DownSample(m_RenderTargetSrvs[RenderTargetTypes::Light].GetAddressOf(), m_RenderTargetUavs[RenderTargetTypes::HalfSize].GetAddressOf(), m_WindowWidth, m_WindowHeight);
	//Pass_DownSample(m_RenderTargetSrvs[RenderTargetTypes::HalfSize].GetAddressOf(), m_RenderTargetUavs[RenderTargetTypes::HalfSize + 1].GetAddressOf(), m_WindowWidth/2 , m_WindowHeight/2);
	//Pass_DownSample(m_RenderTargetSrvs[RenderTargetTypes::HalfSize + 1].GetAddressOf(), m_RenderTargetUavs[RenderTargetTypes::HalfSize + 2].GetAddressOf(), m_WindowWidth/4, m_WindowHeight/4);


	SetPixelShader(m_CompositionShader->GetShader());

	SetPSShaderResources(TextureBindTypes::Position, 1, m_RenderTargetSrvs[RenderTargetTypes::Position].GetAddressOf());
	SetPSShaderResources(TextureBindTypes::Normal, 1, m_RenderTargetSrvs[RenderTargetTypes::Normal].GetAddressOf());
	SetPSShaderResources(TextureBindTypes::Albedo, 1, m_RenderTargetSrvs[RenderTargetTypes::Albedo].GetAddressOf());
	SetPSShaderResources(TextureBindTypes::Material, 1, m_RenderTargetSrvs[RenderTargetTypes::Material].GetAddressOf());
	SetPSShaderResources(TextureBindTypes::Depth, 1, m_RenderTargetSrvs[RenderTargetTypes::Depth].GetAddressOf());
	SetPSShaderResources(TextureBindTypes::ShadowMap, 1, l->GetShadowMapShaderResourceViewAddr());			//shadowmap
	SetPSShaderResources(TextureBindTypes::SSAO, 1, m_RenderTargetSrvs[RenderTargetTypes::SSAO].GetAddressOf());
	SetPSShaderResources(TextureBindTypes::Composition, 1, m_RenderTargetSrvs[RenderTargetTypes::Light].GetAddressOf());
	SetPSShaderResources(TextureBindTypes::ShadowMap, 1, l->GetShadowMapShaderResourceViewAddr());

	SetPSShaderResources(TextureBindTypes::Random, 1, m_RandomTexture.lock()->GetTextureResourceViewAddress());	//random Texture
	SetPSShaderResources(TextureBindTypes::Dithering, 1, m_DitheringTexture.lock()->GetTextureResourceViewAddress());	//Dithering
	SetPSShaderResources(TextureBindTypes::SpecularBRDF, 1, m_IblBrdfTexture.lock()->GetTextureResourceViewAddress());	//specularBRDF_LUT
	SetPSShaderResources(TextureBindTypes::Skybox, 1, m_Skybox->GetCubeMapView());	//skybox
	SetPSShaderResources(TextureBindTypes::IrradianceSkybox, 1, m_Skybox->GetIrMapView());	//Iradiance
	
	
	//SetPSShaderResources(TextureBindTypes::Depth, 1, GetBaseDepthStencilSrvAddress());

	

	RenderQuadPlane();

	SetPSShaderResources(0, DX11Resources::MAX_RENDER_TARGET_BINDING_COUNT + 1, m_NullSrv);
	SetPSShaderResources(9, 1, m_NullSrv);
	SetRenderTarget
	(
		DX11Resources::MAX_RENDER_TARGET_BINDING_COUNT,
		m_NullRtv,
		NULL
	);
}

void Graphics::Render(const std::shared_ptr<RenderInfo>& renderer)
{
	auto & renderables = renderer->GetRenerables();
	renderer->m_IsVisible = false;

	if (renderables.size() == 0) {
		return;
	}

	bool isVisible = IsInViewFrustum(renderer);
	renderer->m_IsVisible = isVisible;
	if (!isVisible) {
		return;
	}

	ApplySkinnedBone(renderer);

	auto& tf = renderer->m_GameObject->GetTransform();
	auto& worldMat = tf.GetWorldMatrix();
	auto wvpMat = worldMat * m_TargetViewProjectionMatrix;
	
	for (auto & r : renderables) {
		ApplyMaterialProperties(r.GetMaterial());
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
	for (auto & mesh : m_Skybox->GetModel()->GetMeshes()) {
		RenderMesh(mesh, worldMat, wvpMat);
	}

	SetRasterizerState(m_RasterizerCullBack.Get());
}

void Graphics::RenderQuadPlane()
{
	auto& mesh = m_QuadWindowModel->GetMeshes()[0];
	SetVertexShader(m_PostProcesVshader->GetShader());
	SetVSInputLayout(m_PostProcesVshader->GetInputLayout());
	SetVertexBuffer(mesh->GetVertexBufferAddr(), mesh->GetVertexBufferStridePtr());
	SetIndexBuffer(mesh->GetIndexBuffer().Get());
	DrawIndexed(mesh->GetIndexBuffer().IndexCount());
}

void Graphics::RenderGizmo(const std::shared_ptr<RenderInfo>& renderer)
{
	if (renderer->m_Model) {
		for (auto& mesh : renderer->m_Model->GetMeshes()) {
			GUI::Draw(m_PrimitiveBatch.get(), Math::GetGlobalBoundingBox(mesh->GetLocalAABB(), renderer->GetGameObject()->GetTransform()));
		}
	}
}

void Graphics::ApplySceneBuffer()
{
	static auto mainCam = Core::GetCurrentScene().GetMainCam();
	static auto light = Core::Find<GameObject>("Light");
	static auto lightc = light->GetComponent<SpotLight>();
	m_GpuSpotLight.data.color = lightc->Color;
	m_GpuSpotLight.data.strength = lightc->Strength;
	m_GpuSpotLight.data.position = light->GetTransform().position;
	m_GpuSpotLight.data.attenuation = lightc->Attentuation;
	DirectX::XMStoreFloat3(&m_GpuSpotLight.data.forwardVector, light->GetTransform().GetForwardVector());
	m_GpuSpotLight.data.spotAngle = lightc->m_SpotAngle * 0.5f * Math::Deg2Rad;
	m_GpuSpotLight.data.range = lightc->m_Range;
	lightc->SetProjectionMatrix();
	m_GpuSpotLight.data.vpMat = lightc->GetLightViewProjectMat();
	DirectX::XMVECTOR center = light->GetTransform().positionVec + light->GetTransform().GetForwardVector() * lightc->m_Range;
	m_GpuSpotLight.data.conePlaneD = DirectX::XMVector3Dot(center, light->GetTransform().GetForwardVector()).m128_f32[0] * -1;
	m_GpuSpotLight.ApplyChanges();

	m_GpuSceneBuffer.data.CamPosition = mainCam->GetTransform().positionVec;
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
	if (m_DrawFlag & DrawFlag::Apply_MaterialVertexShader) {
		auto vs = material->Vshader ? material->Vshader : VertexShader::GetDefault();
		SetVSInputLayout(vs->GetInputLayout());
		SetVertexShader(vs->GetShader());
	}
	if (m_DrawFlag & DrawFlag::Apply_MaterialPixelShader) {
		auto ps = material->Pshader ? material->Pshader : PixelShader::GetDefault();
		SetPixelShader(ps->GetShader());
	}
	if (m_DrawFlag & DrawFlag::Apply_MaterialGeometryShader) {
		auto shader = material->Gshader ? material->Gshader->GetShader() : NULL;
		SetGeometryShader(shader);
	}
	else {
		auto shader = material->Gshader ? GeometryShader::GetDefault()->GetShader() : NULL;
		SetGeometryShader(shader);
	}
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
		SetPSShaderResources(TextureBindTypes::MaterialSpecualr, 1, material->Specular ?
			material->Specular->GetTextureResourceViewAddress() :
			Texture::GetDefault()->GetTextureResourceViewAddress());
	}

	m_GpuMaterialBuffer.data.color = material->Color;
	m_GpuMaterialBuffer.data.NormalIntensity = material->NormalIntensity;
	m_GpuMaterialBuffer.data.RoughnessIntensity = material->RoughnessIntensity;
	m_GpuMaterialBuffer.data.SpecularIntensity = material->SpecularIntensity;
	m_GpuMaterialBuffer.data.MetalIntensity = material->MetalIntensity;
	m_GpuMaterialBuffer.ApplyChanges();
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
	if (m_DrawFlag & DrawFlag::Apply_ViewFrustumCulling) {
		return true;
	}

	bool isVisible = true;

	auto& tf = renderer->m_GameObject->GetTransform();
	for (auto & r : renderer->GetRenerables()) {
		auto globalAABB = Math::GetGlobalBoundingBox(r.GetMesh()->GetLocalAABB(), tf);
		auto containment = m_CullFrustum.Contains(globalAABB);
		isVisible &= containment != DirectX::DISJOINT;
	}

	return isVisible;
}

void Graphics::Pass_PostProcess()
{
	auto inType = RenderTargetTypes::Composition0;
	auto outType = RenderTargetTypes::Composition1;

	Pass_Bloom(inType);

	Pass_ToneMap(inType, outType);
	std::swap(inType, outType);

	Pass_GammaCorrection(inType, outType);
	std::swap(inType, outType);
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
	auto mainCam = Engine::Get().GetCurrentScene().GetMainCam();
	auto spotLight = std::dynamic_pointer_cast<SpotLight>(light);
	auto& lightTransform = spotLight->GetGameObject()->GetTransform();

	m_TargetViewProjectionMatrix = spotLight->GetLightViewProjectMat();
	m_CullFrustum = DirectX::BoundingFrustum(spotLight->GetProjectionMatrix());
	m_CullFrustum.Origin = lightTransform.position;
	DirectX::XMStoreFloat4(&m_CullFrustum.Orientation, lightTransform.GetQuaternion());

	m_DeviceContext->ClearDepthStencilView(m_SubDepthStencilView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	m_DeviceContext->ClearRenderTargetView(spotLight->GetShadowMapRenderTargetView(), m_BackgroundColor);

	SetRenderTarget(1, spotLight->GetShadowMapRenderTargetViewAddr(), m_SubDepthStencilView.Get());
	SetPixelShader(m_ShadowMapPshader->GetShader());

	m_DrawFlag =
		DrawFlag::Apply_MaterialVertexShader |
		DrawFlag::Apply_SkinnedMeshBone;

	static auto drawFunc = std::bind(&Graphics::Render, this, std::placeholders::_1);
	Core::Pool<RenderInfo>::GetInstance().ForEach(drawFunc);

	SetVertexShader(NULL);
	SetPixelShader(NULL);
	SetGeometryShader(NULL);
	SetRenderTarget
	(
		1,
		m_NullRtv,
		NULL
	);
}

void Graphics::Pass_EditorUI()
{
	auto light = Core::Find<GameObject>("Light")->GetComponent<SpotLight>();
	
	SetRenderTarget
	(
		1, 
		m_MainRenderTargetView.GetAddressOf(), 
		NULL
	);

	GUI::NewFrame();
	
	GUI::DrawEditorUI(m_RenderTargetSrvs[DX11Resources::MAX_RENDER_TARGET_BINDING_COUNT].Get());
	
	ImGuiIO& io = ImGui::GetIO();
	ImVec2 scene_size = ImVec2(io.DisplaySize.x * 0.2f, io.DisplaySize.y * 0.2f);

	

	ImGui::Begin("Texture Resources");
	Core::Pool<Texture>::GetInstance().ForEach(GUI::DrawTexture);
	ImGui::End();

	ImGui::Begin("RederTarget Window");
	ImGui::DragFloat("ThresHold", &m_GpuDownSampleBuffer.data.threshold, 0.01f, 0.0f, 10.0f);
	ImGui::Image(m_RenderTargetSrvs[RenderTargetTypes::Composition0].Get(), scene_size);
	ImGui::Image(m_RenderTargetSrvs[RenderTargetTypes::Composition1].Get(), scene_size);
	ImGui::Image(m_RenderTargetSrvs[RenderTargetTypes::HalfQuarterSize].Get(), scene_size);
	ImGui::End();

	GUI::DrawDeferredChannelImage();

	GUI::Render();

	SetRenderTarget
	(
		1,
		m_NullRtv,
		NULL
	);
}

void Graphics::Pass_Gizmo()
{
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

	static auto drawFunc = std::bind(&Graphics::RenderGizmo, this, std::placeholders::_1);
	Core::Pool<RenderInfo>::GetInstance().ForEach(drawFunc);

	m_PrimitiveBatch->End();

	SetRenderTarget
	(
		DX11Resources::MAX_RENDER_TARGET_BINDING_COUNT,
		m_NullRtv,
		NULL
	);
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
}

void Graphics::RenderEnd()
{
	m_Swapchain->Present(1, NULL);
}