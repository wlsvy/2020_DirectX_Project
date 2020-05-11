#include "Graphics.h"

#include "../Engine/Ui.h"
#include <ImGui/imgui_impl_dx11.h>
#include <ImGui/imgui_impl_win32.h>
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
		ThrowIfFailed(DX11Resources::InitializeRenderTarget(width, height),	"Failed to initialize Device Resources RenderTarget");
		ThrowIfFailed(m_GpuObjectBuffer.Initialize(),							"Failed to Initialize CB_VS_vertexshader buffer.");
		ThrowIfFailed(m_GpuBoneBuffer.Initialize(),								"Failed to Initialize cb_vs_BoneInfo buffer.");
		ThrowIfFailed(m_GpuMaterialBuffer.Initialize(),								"Failed to Initialize cb_ps_material buffer.");
		ThrowIfFailed(m_GpuAmbientLightBuffer.Initialize(), "Failed to Initialize cb_ps_ambientLight buffer.");
		ThrowIfFailed(m_GpuSceneBuffer.Initialize(), "Failed to Initialize cb_ps_SceneBase buffer.");
		ThrowIfFailed(m_GpuSpotLight.Initialize(), "Failed to Initialize cb_ps_SpotLight buffer.");
		ThrowIfFailed(m_GpuFurDataBuffer.Initialize(), "Failed to Initialize cb_FurData buffer.");
		ThrowIfFailed(m_GpuBlurBuffer.Initialize(),							"Failed to Initialize cb_cs_ThresholdBlur buffer.");
		ThrowIfFailed(m_GpuDownSampleBuffer.Initialize(), "Failed to Initialize cb_cs_DownSample buffer.");
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

		GUI::InitImGUI(hwnd);

		m_QuadWindowModel = Core::Find<Model>("Plane");
		m_PostProcesVshader = Core::Find<VertexShader>("QuadPlane");
		m_PostProcesPshader = Core::Find<PixelShader>("PostProcess");
		m_ShadowMapPshader = Core::Find<PixelShader>("ShadowMap");
		m_DefaultMaterial = Core::Find<SharedMaterial>("Default");
		m_RandomTexture = Core::Find<Texture>("NoiseNormal");
		m_DitheringTexture = Core::Find<Texture>("Dithering");
		m_IblBrdfTexture = Core::Find<Texture>("ibl_brdf_lut");
		m_FurOpacityTexture = Core::Find<Texture>("Fur_Noise");
		
		return true;
	}
	catch (CustomException & e) {
		StringHelper::ErrorLog(e);
		return false;
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

	static auto mainCam = Core::GetCurrentScene().GetMainCam();
	static auto light = Core::Find<GameObject>("Light");
	static auto lightc = light->GetComponent<SpotLight>();
	m_GpuSpotLight.data.color = lightc->Color;
	m_GpuSpotLight.data.strength = lightc->Strength;
	m_GpuSpotLight.data.position = light->GetTransform().position;
	m_GpuSpotLight.data.attenuation = lightc->Attentuation;
	DirectX::XMStoreFloat3(&m_GpuSpotLight.data.forwardVector, light->GetTransform().GetForwardVector());
	m_GpuSpotLight.data.spotAngle = lightc->m_SpotAngle;
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

	m_DeviceContext->ClearRenderTargetView(m_MainRenderTargetView.Get(), m_BackgroundColor);
	for (int i = 0; i < DX11Resources::MAX_RENDER_TARGET_COUNT; i++) {
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
}

void Graphics::RenderModels()
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


	SetPSShaderResources(6, 1, m_FurOpacityTexture.lock()->GetTextureResourceViewAddress());

	m_DrawFlag = DrawFlag::All;
	static auto drawFunc = std::bind(&Graphics::Render, this, std::placeholders::_1);
	Core::Pool<RenderInfo>::GetInstance().ForEach(drawFunc);

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

void Graphics::Render(const std::shared_ptr<RenderInfo>& renderer)
{
	auto & renderables = renderer->GetRenerables();
	renderer->m_IsVisible = false;

	if (renderables.size() == 0) {
		return;
	}

	bool isVisible = ViewFrustumCull(renderer);
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
		DrawMesh(r.GetMesh(), worldMat, wvpMat);
	}
}

void Graphics::DrawMesh(
	const std::shared_ptr<Mesh>& mesh,
	const DirectX::XMMATRIX & worldMat, 
	const DirectX::XMMATRIX & wvpMat)
{
	m_GpuObjectBuffer.data.wvpMatrix = mesh->GetWorldMatrix() * wvpMat; 
	m_GpuObjectBuffer.data.worldMatrix = mesh->GetWorldMatrix() * worldMat; 
	m_GpuObjectBuffer.ApplyChanges();

	SetVertexBuffer(mesh->GetVertexBufferAddr(), mesh->GetVertexBufferStridePtr());
	SetIndexBuffer(mesh->GetIndexBuffer().Get());

	m_DeviceContext->DrawIndexed(mesh->GetIndexBuffer().IndexCount(), 0, 0);
}

void Graphics::DebugDraw(const std::shared_ptr<RenderInfo>& renderer)
{
	if (renderer->m_Model) {
		for (auto& mesh : renderer->m_Model->GetMeshes()) {
			GUI::Draw(m_PrimitiveBatch.get(), Math::GetGlobalBoundingBox(mesh->GetLocalAABB(), renderer->GetGameObject()->GetTransform()));
		}
	}
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
		SetPSShaderResources(0, 1, material->Albedo ?
			material->Albedo->GetTextureResourceViewAddress() :
			Texture::GetDefault()->GetTextureResourceViewAddress());
		SetPSShaderResources(1, 1, material->Normal ?
			material->Normal->GetTextureResourceViewAddress() :
			Texture::GetDefault()->GetTextureResourceViewAddress());
		SetPSShaderResources(2, 1, material->Metal ?
			material->Metal->GetTextureResourceViewAddress() :
			Texture::GetDefault()->GetTextureResourceViewAddress());
		SetPSShaderResources(3, 1, material->Roughness ?
			material->Roughness->GetTextureResourceViewAddress() :
			Texture::GetDefault()->GetTextureResourceViewAddress());
		SetPSShaderResources(4, 1, material->Specular ?
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

bool Graphics::ViewFrustumCull(const std::shared_ptr<RenderInfo>& renderer)
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

void Graphics::PostProcess()
{
	auto l = Core::Find<GameObject>("Light")->GetComponent<SpotLight>();

	SetRenderTarget
	(
		1, 
		m_RenderTargetViewArr[DX11Resources::MAX_RENDER_TARGET_BINDING_COUNT].GetAddressOf(), 
		NULL
	);
	SetVertexShader(m_PostProcesVshader->GetShader());
	SetPixelShader(m_PostProcesPshader->GetShader());

	SetPSShaderResources(0, 1, m_ShaderResourceViewArr[0].GetAddressOf());	//pos
	SetPSShaderResources(1, 1, m_ShaderResourceViewArr[1].GetAddressOf());	//normal
	SetPSShaderResources(2, 1, m_ShaderResourceViewArr[2].GetAddressOf());	//albedo
	SetPSShaderResources(3, 1, m_ShaderResourceViewArr[3].GetAddressOf());	//Mat
	SetPSShaderResources(4, 1, m_ShaderResourceViewArr[4].GetAddressOf());	//depth

	SetPSShaderResources(5, 1, l->GetShadowMapShaderResourceViewAddr());			//shadowmap
	SetPSShaderResources(8, 1, m_RandomTexture.lock()->GetTextureResourceViewAddress());	//random Texture
	SetPSShaderResources(9, 1, m_Skybox->GetCubeMapView());	//skybox
	SetPSShaderResources(10, 1, m_DitheringTexture.lock()->GetTextureResourceViewAddress());	//Dithering
	SetPSShaderResources(11, 1, m_Skybox->GetIrMapView());	//Iradiance
	SetPSShaderResources(12, 1, m_IblBrdfTexture.lock()->GetTextureResourceViewAddress());	//specularBRDF_LUT

	//SetPSShaderResources(4, 1, m_DeviceResources.GetBaseDepthStencilSrvAddress());	//depth
	//SetPSShaderResources(4, 1, m_DeviceResources.GetRenderTargetSrvAddress(3));	//blur 계산이후



	auto& mesh = m_QuadWindowModel->GetMeshes()[0];
	UINT offset = 0;
	SetVertexBuffer(mesh->GetVertexBufferAddr(), mesh->GetVertexBufferStridePtr());
	SetIndexBuffer(mesh->GetIndexBuffer().Get());
	m_DeviceContext->DrawIndexed(mesh->GetIndexBuffer().IndexCount(), 0, 0);

	SetPSShaderResources(0, DX11Resources::MAX_RENDER_TARGET_BINDING_COUNT + 1, m_NullSrv);
	SetPSShaderResources(9, 1, m_NullSrv);
	SetRenderTarget
	(
		DX11Resources::MAX_RENDER_TARGET_BINDING_COUNT, 
		m_NullRtv, 
		NULL
	);
}

void Graphics::DrawSkybox()
{
	SetRenderTarget
	(
		DX11Resources::MAX_RENDER_TARGET_BINDING_COUNT,
		m_RenderTargetViewArr[0].GetAddressOf(),
		m_MainDepthStencilView.Get()
	);

	SetVSInputLayout(m_Skybox->GetVertexShader()->GetInputLayout());
	SetVertexShader(m_Skybox->GetVertexShader()->GetShader());
	SetPixelShader(m_Skybox->GetPixelShader()->GetShader());
	SetPSShaderResources(1, 1, m_Skybox->GetCubeMapView());

	SetRasterizerState(m_Skybox->GetRasterizerState());
	SetDepthStencilState(m_Skybox->GetDepthStencilState());

	m_DrawFlag = DrawFlag::None;

	auto mainCam = Engine::Get().GetCurrentScene().GetMainCam();
	auto worldMat = DirectX::XMMatrixTranslationFromVector(mainCam->GetTransform().positionVec);
	auto wvpMat = worldMat * mainCam->GetViewProjectionMatrix();
	for (auto & mesh : m_Skybox->GetModel()->GetMeshes()) {
		DrawMesh(mesh, worldMat, wvpMat);
	}

	SetRasterizerState(m_RasterizerCullBack.Get());
	SetDepthStencilState(m_DepthStencilState.Get());

	SetRenderTarget
	(
		DX11Resources::MAX_RENDER_TARGET_BINDING_COUNT,
		m_NullRtv,
		NULL
	);
}

void Graphics::DrawShadowMap(const std::shared_ptr<LightBase> & light)
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

void Graphics::DrawGui()
{
	auto light = Core::Find<GameObject>("Light")->GetComponent<SpotLight>();
	
	SetRenderTarget
	(
		1, 
		m_MainRenderTargetView.GetAddressOf(), 
		NULL
	);

	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
	GUI::DrawEditorUI(m_ShaderResourceViewArr[DX11Resources::MAX_RENDER_TARGET_BINDING_COUNT].Get());
	
	ImGuiIO& io = ImGui::GetIO();
	ImVec2 scene_size = ImVec2(io.DisplaySize.x * 0.2f, io.DisplaySize.y * 0.2f);

	

	ImGui::Begin("Texture Resources");
	Core::Pool<Texture>::GetInstance().ForEach(GUI::DrawTexture);
	ImGui::End();

	ImGui::Begin("Blur Test");
	ImGui::DragFloat("ThresHold", &m_GpuDownSampleBuffer.data.threshold, 0.01f, 0.0f, 10.0f);
	ImGui::Image(m_ShaderResourceViewArr[3].Get(), scene_size);
	ImGui::Image(m_ShaderResourceViewArr[5].Get(), scene_size);
	ImGui::Image(m_ShaderResourceViewArr[6].Get(), scene_size);
	ImGui::Image(m_Skybox->GetCubeMapSrv(), scene_size);
	ImGui::Image(m_Skybox->GetIrMapSrv(), scene_size);

	ImGui::End();

	GUI::DrawDeferredChannelImage();

	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

	SetRenderTarget
	(
		1,
		m_NullRtv,
		NULL
	);
}

void Graphics::DrawGuiDebug()
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

	static auto drawFunc = std::bind(&Graphics::DebugDraw, this, std::placeholders::_1);
	Core::Pool<RenderInfo>::GetInstance().ForEach(drawFunc);

	m_PrimitiveBatch->End();

	SetRenderTarget
	(
		DX11Resources::MAX_RENDER_TARGET_BINDING_COUNT,
		m_NullRtv,
		NULL
	);
}

void Graphics::ComputeShdaderTest()
{
	static auto csBlur = Core::Find<ComputeShader>("Blur");
	static auto csDownsample = Core::Find<ComputeShader>("ThresholdDownSample");
	auto light = Core::Find<GameObject>("Light")->GetComponent<SpotLight>();
	
	{
		m_GpuBlurBuffer.data.radius = GpuBlurBuffer::GAUSSIAN_RADIUS;

		// compute Gaussian kernel
		float sigma = 10.f;
		float sigmaRcp = 1.f / sigma;
		float twoSigmaSq = 2 * sigma * sigma;

		float sum = 0.f;
		for (UINT i = 0; i <= GpuBlurBuffer::GAUSSIAN_RADIUS; ++i)
		{
			// we omit the normalization factor here for the discrete version and normalize using the sum afterwards
			m_GpuBlurBuffer.data.coefficients[i] = (1.f / sigma) * std::expf(-static_cast<float>(i * i) / twoSigmaSq);
			// we use each entry twice since we only compute one half of the curve
			sum += 2 * m_GpuBlurBuffer.data.coefficients[i];
		}
		// the center (index 0) has been counted twice, so we subtract it once
		sum -= m_GpuBlurBuffer.data.coefficients[0];

		// we normalize all entries using the sum so that the entire kernel gives us a sum of coefficients = 0
		float normalizationFactor = 1.f / sum;
		for (UINT i = 0; i <= GpuBlurBuffer::GAUSSIAN_RADIUS; ++i)
		{
			m_GpuBlurBuffer.data.coefficients[i] *= normalizationFactor;
		}
		m_GpuBlurBuffer.ApplyChanges();
		
	}

	{
		m_GpuDownSampleBuffer.ApplyChanges();
		m_DeviceContext->CSSetShader(csDownsample->GetShader(), 0, 0);
		m_DeviceContext->CSSetConstantBuffers(0, 1, m_GpuDownSampleBuffer.GetAddressOf());
		m_DeviceContext->CSSetShaderResources(0, 1, m_ShaderResourceViewArr[3].GetAddressOf());
		m_DeviceContext->CSSetUnorderedAccessViews(0, 1, m_UnorderedAccessView[5].GetAddressOf(), 0);

		m_DeviceContext->Dispatch(m_WindowWidth / 16, m_WindowHeight / 16, 1);

		m_DeviceContext->CSSetUnorderedAccessViews(0, 1, m_NullUav, 0);
		m_DeviceContext->CSSetShaderResources(0, 1, m_NullSrv);
	}
	{
		m_DeviceContext->CSSetShader(csBlur->GetShader(), 0, 0);
		ID3D11ShaderResourceView* csSRVs[2] = { 
			m_ShaderResourceViewArr[5].Get(), 
			m_ShaderResourceViewArr[6].Get() };
		ID3D11UnorderedAccessView* csUAVs[2] = {
			m_UnorderedAccessView[6].Get(),
			m_UnorderedAccessView[5].Get() };

		for (UINT direction = 0; direction < 2; ++direction)
		{
			m_GpuBlurBuffer.data.direction = direction;
			m_GpuBlurBuffer.ApplyChanges();
			m_DeviceContext->CSSetConstantBuffers(0, 1, m_GpuBlurBuffer.GetAddressOf());
			m_DeviceContext->CSSetShaderResources(0, 1, &csSRVs[direction]);
			m_DeviceContext->CSSetUnorderedAccessViews(0, 1, &csUAVs[direction], 0);

			m_DeviceContext->Dispatch(m_WindowWidth / 16, m_WindowHeight / 16, 1);

			m_DeviceContext->CSSetShaderResources(0, 1, m_NullSrv);
			m_DeviceContext->CSSetUnorderedAccessViews(0, 1, m_NullUav, 0);
		}
	}
}

void Graphics::RenderEnd()
{
	m_Swapchain->Present(1, NULL);
}