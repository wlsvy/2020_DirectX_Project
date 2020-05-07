#include "Graphics.h"

#include "../Engine/Ui.h"
//#include <ImGui/imgui_impl_dx11.h>
//#include <ImGui/imgui_impl_win32.h>
#include "imGui/imgui_impl_win32.h"
#include "imGui/imgui_impl_dx11.h"
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

bool Graphics::Initialize(HWND hwnd, UINT width, UINT height) {
	try {
		windowWidth = width;
		windowHeight = height;

		ThrowIfFailed(DX11Resources::Initialize(hwnd, width, height),		"Failed to initialize Device Resources");
		ThrowIfFailed(DX11Resources::InitializeRenderTarget(width, height),	"Failed to initialize Device Resources RenderTarget");
		ThrowIfFailed(cb_vs_vertexshader_2d.Initialize(),						"Failed to Initialize CB_VS_vertexshader_2d buffer.");
		ThrowIfFailed(cb_vs_vertexshader.Initialize(),							"Failed to Initialize CB_VS_vertexshader buffer.");
		ThrowIfFailed(cb_vs_BoneInfo.Initialize(),								"Failed to Initialize cb_vs_BoneInfo buffer.");
		ThrowIfFailed(cb_ps_material.Initialize(),								"Failed to Initialize cb_ps_material buffer.");
		ThrowIfFailed(cb_ps_ambientLight.Initialize(), "Failed to Initialize cb_ps_ambientLight buffer.");
		ThrowIfFailed(cb_ps_SceneBase.Initialize(), "Failed to Initialize cb_ps_SceneBase buffer.");
		ThrowIfFailed(cb_ps_SpotLight.Initialize(), "Failed to Initialize cb_ps_SpotLight buffer.");
		ThrowIfFailed(cb_FurData.Initialize(), "Failed to Initialize cb_FurData buffer.");
		ThrowIfFailed(cb_cs_ThresholdBlur.Initialize(),							"Failed to Initialize cb_cs_ThresholdBlur buffer.");
		ThrowIfFailed(cb_cs_DownSample.Initialize(), "Failed to Initialize cb_cs_DownSample buffer.");
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

		m_PostProcesWindowModel = Core::Find<Model>("Plane");
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
	using DirectX::operator+;
	using DirectX::operator*;
	static auto mainCam = Core::GetCurrentScene().GetMainCam();
	static auto light = Core::Find<GameObject>("Light");
	static auto lightc = light->GetComponent<SpotLight>();
	cb_ps_SpotLight.data.color = lightc->Color;
	cb_ps_SpotLight.data.strength = lightc->Strength;
	cb_ps_SpotLight.data.position = light->GetTransform().position;
	cb_ps_SpotLight.data.attenuation = lightc->Attentuation;
	DirectX::XMStoreFloat3(&cb_ps_SpotLight.data.forwardVector, light->GetTransform().GetForwardVector());
	cb_ps_SpotLight.data.spotAngle = lightc->m_SpotAngle;
	cb_ps_SpotLight.data.range = lightc->m_Range;
	lightc->SetProjectionMatrix();
	cb_ps_SpotLight.data.vpMat = lightc->GetLightViewProjectMat();
	DirectX::XMVECTOR center = light->GetTransform().positionVec + light->GetTransform().GetForwardVector() * lightc->m_Range;
	cb_ps_SpotLight.data.conePlaneD = DirectX::XMVector3Dot(center, light->GetTransform().GetForwardVector()).m128_f32[0] * -1;
	cb_ps_SpotLight.ApplyChanges();

	cb_ps_SceneBase.data.CamPosition = mainCam->GetTransform().positionVec;
	cb_ps_SceneBase.data.CameraForward = mainCam->GetTransform().GetForwardVector();
	cb_ps_SceneBase.data.ElapsedTime = Time::GetTime();
	cb_ps_SceneBase.data.DeltaTime = Time::GetDeltaTime();
	cb_ps_SceneBase.data.InverseViewMat = DirectX::XMMatrixInverse(nullptr, mainCam->GetViewMatrix());
	cb_ps_SceneBase.data.InverseProjMat = DirectX::XMMatrixInverse(nullptr, mainCam->GetProjectionMatrix());
	cb_ps_SceneBase.ApplyChanges();

	cb_FurData.ApplyChanges();

	m_DeviceContext->GSSetConstantBuffers(0, 1, cb_vs_vertexshader.GetAddressOf());
	m_DeviceContext->GSSetConstantBuffers(1, 1, cb_FurData.GetAddressOf());

	m_DeviceContext->PSSetConstantBuffers(0, 1, cb_ps_SceneBase.GetAddressOf());
	m_DeviceContext->PSSetConstantBuffers(1, 1, cb_ps_SpotLight.GetAddressOf());
	m_DeviceContext->PSSetConstantBuffers(2, 1, cb_ps_material.GetAddressOf());
	m_DeviceContext->PSSetConstantBuffers(3, 1, cb_FurData.GetAddressOf());

	m_DeviceContext->VSSetConstantBuffers(0, 1, cb_vs_vertexshader.GetAddressOf());
	m_DeviceContext->VSSetConstantBuffers(1, 1, cb_vs_BoneInfo.GetAddressOf());

	m_DeviceContext->ClearRenderTargetView(m_MainRenderTargetView.Get(), m_BackgroundColor);
	for (int i = 0; i < DX11Resources::RenderTargetCount; i++) {
		m_DeviceContext->ClearRenderTargetView(m_RenderTargetViewArr[i].Get(), m_BackgroundColor);
	}
	m_DeviceContext->ClearDepthStencilView(m_MainDepthStencilView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	m_DeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY::D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	m_DeviceContext->RSSetState(m_RasterizerState.Get());
	m_DeviceContext->OMSetDepthStencilState(m_DepthStencilState.Get(), 0);
	m_DeviceContext->OMSetBlendState(m_BlendState.Get(), m_BlendFactors, 0xFFFFFFFF);
	auto samplerClamp = m_CommonState->PointClamp();
	auto samplerWrap = m_CommonState->LinearWrap();
	auto samplerAntioWrap = m_CommonState->AnisotropicWrap();
	m_DeviceContext->PSSetSamplers(0, 1, &samplerClamp);
	m_DeviceContext->PSSetSamplers(1, 1, &samplerWrap);
	m_DeviceContext->PSSetSamplers(2, 1, &samplerAntioWrap);
}

void Graphics::RenderModels()
{
	auto mainCam = Engine::Get().GetCurrentScene().GetMainCam();
	m_TargetViewProjectionMatrix = mainCam->GetViewProjectionMatrix();
	m_CullFrustum = mainCam->GetViewFrustum();

	m_DeviceContext->OMSetRenderTargets(
		DX11Resources::DeferredRenderChannelCount, 
		m_RenderTargetViewArr[0].GetAddressOf(), 
		m_MainDepthStencilView.Get());

	m_DeviceContext->PSSetShaderResources(6, 1, m_FurOpacityTexture.lock()->GetTextureResourceViewAddress());	//pos


	m_DrawFlag = DrawFlag::All;
	static auto drawFunc = std::bind(&Graphics::Render, this, std::placeholders::_1);
	Core::Pool<RenderInfo>::GetInstance().ForEach(drawFunc);

	m_DeviceContext->OMSetRenderTargets(
		DX11Resources::DeferredRenderChannelCount,
		m_NullRtv,
		NULL);
	m_DeviceContext->VSSetShader(NULL, NULL, 0);
	m_DeviceContext->PSSetShader(NULL, NULL, 0);
	m_DeviceContext->GSSetShader(NULL, NULL, 0);
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
	cb_vs_vertexshader.data.wvpMatrix = mesh->GetWorldMatrix() * wvpMat; 
	cb_vs_vertexshader.data.worldMatrix = mesh->GetWorldMatrix() * worldMat; 
	cb_vs_vertexshader.ApplyChanges();

	UINT offset = 0;
	m_DeviceContext->IASetVertexBuffers(0, 1, mesh->GetVertexBufferAddr(), mesh->GetVertexBufferStridePtr(), &offset);
	m_DeviceContext->IASetIndexBuffer(mesh->GetIndexBuffer().Get(), DXGI_FORMAT::DXGI_FORMAT_R32_UINT, 0);
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
		m_DeviceContext->IASetInputLayout(vs->GetInputLayout());
		m_DeviceContext->VSSetShader(vs->GetShader(), NULL, 0);
	}
	if (m_DrawFlag & DrawFlag::Apply_MaterialPixelShader) {
		auto ps = material->Pshader ? material->Pshader : PixelShader::GetDefault();
		m_DeviceContext->PSSetShader(ps->GetShader(), NULL, 0);
	}
	if (m_DrawFlag & DrawFlag::Apply_MaterialGeometryShader) {
		auto shader = material->Gshader ? material->Gshader->GetShader() : NULL;
		m_DeviceContext->GSSetShader(shader, NULL, 0);
	}
	else {
		auto shader = material->Gshader ? GeometryShader::GetDefault()->GetShader() : NULL;
		m_DeviceContext->GSSetShader(shader, NULL, 0);
	}
	if (m_DrawFlag & DrawFlag::Apply_MaterialTexture){
		m_DeviceContext->PSSetShaderResources(0, 1, material->Albedo ?
			material->Albedo->GetTextureResourceViewAddress() : 
			Texture::GetDefault()->GetTextureResourceViewAddress());

		m_DeviceContext->PSSetShaderResources(1, 1, material->Normal ?
			material->Normal->GetTextureResourceViewAddress() :
			Texture::GetDefault()->GetTextureResourceViewAddress());

		m_DeviceContext->PSSetShaderResources(2, 1, material->Metal ?
			material->Metal->GetTextureResourceViewAddress() :
			Texture::GetDefault()->GetTextureResourceViewAddress());

		m_DeviceContext->PSSetShaderResources(3, 1, material->Roughness ?
			material->Roughness->GetTextureResourceViewAddress() :
			Texture::GetDefault()->GetTextureResourceViewAddress());

		m_DeviceContext->PSSetShaderResources(4, 1, material->Specular ?
			material->Specular->GetTextureResourceViewAddress() :
			Texture::GetDefault()->GetTextureResourceViewAddress());
	}

	cb_ps_material.data.color = material->Color;
	cb_ps_material.data.NormalIntensity = material->NormalIntensity;
	cb_ps_material.data.RoughnessIntensity = material->RoughnessIntensity;
	cb_ps_material.data.SpecularIntensity = material->SpecularIntensity;
	cb_ps_material.data.MetalIntensity = material->MetalIntensity;
	cb_ps_material.ApplyChanges();
}

void Graphics::ApplySkinnedBone(const std::shared_ptr<RenderInfo>& renderer)
{
	if (m_DrawFlag & DrawFlag::Apply_SkinnedMeshBone &&
		renderer->Anim &&
		renderer->Anim->GetClip())
	{
		CopyMemory(cb_vs_BoneInfo.data.boneTransform,
			renderer->Anim->GetAnimResult().data(),
			renderer->Anim->GetAnimResult().size() * sizeof(DirectX::XMMATRIX));
		cb_vs_BoneInfo.ApplyChanges();
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

	m_DeviceContext->OMSetRenderTargets(1, m_RenderTargetViewArr[DX11Resources::DeferredRenderChannelCount].GetAddressOf(), NULL);

	m_DeviceContext->VSSetShader(m_PostProcesVshader->GetShader(), NULL, 0);
	m_DeviceContext->PSSetShader(m_PostProcesPshader->GetShader(), NULL, 0);

	m_DeviceContext->PSSetShaderResources(0, 1, m_ShaderResourceViewArr[0].GetAddressOf());	//pos
	m_DeviceContext->PSSetShaderResources(1, 1, m_ShaderResourceViewArr[1].GetAddressOf());	//normal
	m_DeviceContext->PSSetShaderResources(2, 1, m_ShaderResourceViewArr[2].GetAddressOf());	//albedo
	m_DeviceContext->PSSetShaderResources(3, 1, m_ShaderResourceViewArr[3].GetAddressOf());	//Mat
	m_DeviceContext->PSSetShaderResources(4, 1, m_ShaderResourceViewArr[4].GetAddressOf());	//depth
	m_DeviceContext->PSSetShaderResources(5, 1, l->GetShadowMapShaderResourceViewAddr());			//shadowmap

	m_DeviceContext->PSSetShaderResources(8, 1, m_RandomTexture.lock()->GetTextureResourceViewAddress());	//random Texture
	m_DeviceContext->PSSetShaderResources(9, 1, m_Skybox->GetCubeMapView());	//skybox
	m_DeviceContext->PSSetShaderResources(10, 1, m_DitheringTexture.lock()->GetTextureResourceViewAddress());	//Dithering
	m_DeviceContext->PSSetShaderResources(11, 1, m_Skybox->GetIrMapView());	//Iradiance
	m_DeviceContext->PSSetShaderResources(12, 1, m_IblBrdfTexture.lock()->GetTextureResourceViewAddress());	//specularBRDF_LUT

	//m_DeviceContext->PSSetShaderResources(4, 1, m_DeviceResources.GetBaseDepthStencilSrvAddress());	//depth
	//m_DeviceContext->PSSetShaderResources(4, 1, m_DeviceResources.GetRenderTargetSrvAddress(3));	//blur 계산이후



	auto& mesh = m_PostProcesWindowModel->GetMeshes()[0];
	UINT offset = 0;
	m_DeviceContext->IASetVertexBuffers(0, 1, mesh->GetVertexBufferAddr(), mesh->GetVertexBufferStridePtr(), &offset);
	m_DeviceContext->IASetIndexBuffer(mesh->GetIndexBuffer().Get(), DXGI_FORMAT::DXGI_FORMAT_R32_UINT, 0);
	m_DeviceContext->DrawIndexed(mesh->GetIndexBuffer().IndexCount(), 0, 0);

	m_DeviceContext->PSSetShaderResources(0, DX11Resources::DeferredRenderChannelCount + 1, m_NullSrv);
	m_DeviceContext->PSSetShaderResources(9, 1, m_NullSrv);
	m_DeviceContext->OMSetRenderTargets(DX11Resources::DeferredRenderChannelCount, m_NullRtv, NULL);
}

void Graphics::DrawSkybox()
{
	m_DeviceContext->OMSetRenderTargets(
		DX11Resources::DeferredRenderChannelCount,
		m_RenderTargetViewArr[0].GetAddressOf(),
		m_MainDepthStencilView.Get());

	m_DeviceContext->IASetInputLayout(m_Skybox->GetVertexShader()->GetInputLayout());
	m_DeviceContext->VSSetShader(m_Skybox->GetVertexShader()->GetShader(), NULL, 0);
	m_DeviceContext->PSSetShader(m_Skybox->GetPixelShader()->GetShader(), NULL, 0);
	m_DeviceContext->PSSetShaderResources(1, 1, m_Skybox->GetCubeMapView());

	m_DeviceContext->RSSetState(m_Skybox->GetRasterizerState());
	m_DeviceContext->OMSetDepthStencilState(m_Skybox->GetDepthStencilState(), 0);

	m_DrawFlag = DrawFlag::None;

	auto mainCam = Engine::Get().GetCurrentScene().GetMainCam();
	auto worldMat = DirectX::XMMatrixTranslationFromVector(mainCam->GetTransform().positionVec);
	auto wvpMat = worldMat * mainCam->GetViewProjectionMatrix();
	for (auto & mesh : m_Skybox->GetModel()->GetMeshes()) {
		DrawMesh(mesh, worldMat, wvpMat);
	}

	m_DeviceContext->RSSetState(m_RasterizerState.Get());
	m_DeviceContext->OMSetDepthStencilState(m_DepthStencilState.Get(), 0);

	m_DeviceContext->OMSetRenderTargets(
		DX11Resources::DeferredRenderChannelCount,
		m_NullRtv,
		NULL);
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
	m_DeviceContext->OMSetRenderTargets(1, spotLight->GetShadowMapRenderTargetViewAddr(), m_SubDepthStencilView.Get());
	m_DeviceContext->PSSetShader(m_ShadowMapPshader->GetShader(), NULL, 0);

	m_DrawFlag =
		DrawFlag::Apply_MaterialVertexShader |
		DrawFlag::Apply_SkinnedMeshBone;

	static auto drawFunc = std::bind(&Graphics::Render, this, std::placeholders::_1);
	Core::Pool<RenderInfo>::GetInstance().ForEach(drawFunc);

	m_DeviceContext->OMSetRenderTargets(1, m_NullRtv, NULL);
	m_DeviceContext->VSSetShader(NULL, NULL, 0);
	m_DeviceContext->PSSetShader(NULL, NULL, 0);
	m_DeviceContext->GSSetShader(NULL, NULL, 0);
}

void Graphics::DrawGui()
{
	auto light = Core::Find<GameObject>("Light")->GetComponent<SpotLight>();
	
	m_DeviceContext->OMSetRenderTargets(1, m_MainRenderTargetView.GetAddressOf(), NULL);

	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
	GUI::DrawEditorUI(m_ShaderResourceViewArr[DX11Resources::DeferredRenderChannelCount].Get());
	
	ImGuiIO& io = ImGui::GetIO();
	ImVec2 scene_size = ImVec2(io.DisplaySize.x * 0.2f, io.DisplaySize.y * 0.2f);

	

	ImGui::Begin("Texture Resources");
	Core::Pool<Texture>::GetInstance().ForEach(GUI::DrawTexture);
	ImGui::End();

	ImGui::Begin("Blur Test");
	ImGui::DragFloat("ThresHold", &cb_cs_DownSample.data.threshold, 0.01f, 0.0f, 10.0f);
	ImGui::Image(m_ShaderResourceViewArr[3].Get(), scene_size);
	ImGui::Image(m_ShaderResourceViewArr[5].Get(), scene_size);
	ImGui::Image(m_ShaderResourceViewArr[6].Get(), scene_size);
	ImGui::Image(m_Skybox->GetCubeMapSrv(), scene_size);
	ImGui::Image(m_Skybox->GetIrMapSrv(), scene_size);

	ImGui::End();

	GUI::DrawDeferredChannelImage();

	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

	m_DeviceContext->OMSetRenderTargets(1, m_NullRtv, NULL);
}

void Graphics::DrawGuiDebug()
{
	auto& camera = *Engine::Get().GetCurrentScene().GetMainCam();

	m_BasicEffect->SetVertexColorEnabled(true);
	m_BasicEffect->SetView(camera.GetViewMatrix());
	m_BasicEffect->SetProjection(camera.GetProjectionMatrix());

	m_DeviceContext->OMSetBlendState(m_CommonState->Opaque(), nullptr, 0xFFFFFFFF);
	m_DeviceContext->OMSetDepthStencilState(m_CommonState->DepthNone(), 0);
	m_DeviceContext->RSSetState(m_CommonState->CullNone());

	m_BasicEffect->Apply(m_DeviceContext.Get());

	m_DeviceContext->IASetInputLayout(m_DebugInputLayout.Get());

	m_DeviceContext->OMSetRenderTargets(
		1,
		m_RenderTargetViewArr[DX11Resources::DeferredRenderChannelCount].GetAddressOf(),
		m_MainDepthStencilView.Get());

	m_PrimitiveBatch->Begin();

	static auto drawFunc = std::bind(&Graphics::DebugDraw, this, std::placeholders::_1);
	Core::Pool<RenderInfo>::GetInstance().ForEach(drawFunc);

	m_PrimitiveBatch->End();

	m_DeviceContext->OMSetRenderTargets(
		DX11Resources::DeferredRenderChannelCount,
		m_NullRtv,
		NULL);
}

void Graphics::ComputeShdaderTest()
{
	static auto csBlur = Core::Find<ComputeShader>("Blur");
	static auto csDownsample = Core::Find<ComputeShader>("ThresholdDownSample");
	auto light = Core::Find<GameObject>("Light")->GetComponent<SpotLight>();
	
	{
		cb_cs_ThresholdBlur.data.radius = CB_CS_ThresholdBlur::GAUSSIAN_RADIUS;

		// compute Gaussian kernel
		float sigma = 10.f;
		float sigmaRcp = 1.f / sigma;
		float twoSigmaSq = 2 * sigma * sigma;

		float sum = 0.f;
		for (UINT i = 0; i <= CB_CS_ThresholdBlur::GAUSSIAN_RADIUS; ++i)
		{
			// we omit the normalization factor here for the discrete version and normalize using the sum afterwards
			cb_cs_ThresholdBlur.data.coefficients[i] = (1.f / sigma) * std::expf(-static_cast<float>(i * i) / twoSigmaSq);
			// we use each entry twice since we only compute one half of the curve
			sum += 2 * cb_cs_ThresholdBlur.data.coefficients[i];
		}
		// the center (index 0) has been counted twice, so we subtract it once
		sum -= cb_cs_ThresholdBlur.data.coefficients[0];

		// we normalize all entries using the sum so that the entire kernel gives us a sum of coefficients = 0
		float normalizationFactor = 1.f / sum;
		for (UINT i = 0; i <= CB_CS_ThresholdBlur::GAUSSIAN_RADIUS; ++i)
		{
			cb_cs_ThresholdBlur.data.coefficients[i] *= normalizationFactor;
		}
		cb_cs_ThresholdBlur.ApplyChanges();
		
	}

	{
		cb_cs_DownSample.ApplyChanges();
		m_DeviceContext->CSSetShader(csDownsample->GetShader(), 0, 0);
		m_DeviceContext->CSSetConstantBuffers(0, 1, cb_cs_DownSample.GetAddressOf());
		m_DeviceContext->CSSetShaderResources(0, 1, m_ShaderResourceViewArr[3].GetAddressOf());
		m_DeviceContext->CSSetUnorderedAccessViews(0, 1, m_UnorderedAccessView[5].GetAddressOf(), 0);

		m_DeviceContext->Dispatch(windowWidth / 16, windowHeight / 16, 1);

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
			cb_cs_ThresholdBlur.data.direction = direction;
			cb_cs_ThresholdBlur.ApplyChanges();
			m_DeviceContext->CSSetConstantBuffers(0, 1, cb_cs_ThresholdBlur.GetAddressOf());
			m_DeviceContext->CSSetShaderResources(0, 1, &csSRVs[direction]);
			m_DeviceContext->CSSetUnorderedAccessViews(0, 1, &csUAVs[direction], 0);

			m_DeviceContext->Dispatch(windowWidth / 16, windowHeight / 16, 1);

			m_DeviceContext->CSSetShaderResources(0, 1, m_NullSrv);
			m_DeviceContext->CSSetUnorderedAccessViews(0, 1, m_NullUav, 0);
		}
	}
}

void Graphics::RenderEnd()
{
	m_Swapchain->Present(1, NULL);
}