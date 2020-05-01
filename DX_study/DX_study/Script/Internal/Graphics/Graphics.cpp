#include "Graphics.h"

#include "imGui/imgui.h"
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
#include "../Engine/DeviceResources.h"
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

bool Graphics::Initialize(HWND hwnd, int width, int height) {
	try {
		windowWidth = width;
		windowHeight = height;

		ThrowIfFailed(m_DeviceResources.Initialize(hwnd, width, height),		"Failed to initialize Device Resources");
		ThrowIfFailed(m_DeviceResources.InitializeRenderTarget(width, height),	"Failed to initialize Device Resources RenderTarget");
		ThrowIfFailed(cb_vs_vertexshader_2d.Initialize(),						"Failed to Initialize CB_VS_vertexshader_2d buffer.");
		ThrowIfFailed(cb_vs_vertexshader.Initialize(),							"Failed to Initialize CB_VS_vertexshader buffer.");
		ThrowIfFailed(cb_vs_BoneInfo.Initialize(),								"Failed to Initialize cb_vs_BoneInfo buffer.");
		ThrowIfFailed(cb_ps_material.Initialize(),								"Failed to Initialize cb_ps_material buffer.");
		ThrowIfFailed(cb_ps_ambientLight.Initialize(), "Failed to Initialize cb_ps_ambientLight buffer.");
		ThrowIfFailed(cb_ps_SceneBase.Initialize(), "Failed to Initialize cb_ps_SceneBase buffer.");
		ThrowIfFailed(cb_ps_SpotLight.Initialize(), "Failed to Initialize cb_ps_SpotLight buffer.");
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
		m_PostProcesVshader = Core::Find<VertexShader>("vertexshader_WindowPlane");
		m_PostProcesPshader = Core::Find<PixelShader>("pixelshader_PostProcess");
		m_ShadowMapPshader = Core::Find<PixelShader>("pixelshader_shadowMapDepth");
		m_DefaultMaterial = Core::Find<SharedMaterial>("Default");
		m_DefaultTexture = Core::Find<Texture>("WhiteTexture");
		m_SSAOrefTexture = Core::Find<Texture>("NoiseNormal");
		return true;
	}
	catch (CustomException & e) {
		ErrorLogger::Log(e);
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
			material->MainTexture = Core::Find<Texture>(table["Texture"][i]);

			auto splitted = Importer::SplitString(table["Color"][i], '/');
			material->Color.x = std::stof(splitted[0]) / 255;
			material->Color.y = std::stof(splitted[1]) / 255;
			material->Color.z = std::stof(splitted[2]) / 255;
			material->Color.w = std::stof(splitted[3]) / 255;
		}
		return true;
	}
	catch (CustomException &e) {
		ErrorLogger::Log(e);
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
	cb_ps_SceneBase.data.InverseViewMat = DirectX::XMMatrixInverse(nullptr, mainCam->GetViewMatrix());
	cb_ps_SceneBase.data.InverseProjMat = DirectX::XMMatrixInverse(nullptr, mainCam->GetProjectionMatrix());
	cb_ps_SceneBase.ApplyChanges();

	m_DeviceResources.GetDeviceContext()->PSSetConstantBuffers(0, 1, cb_ps_SceneBase.GetAddressOf());
	m_DeviceResources.GetDeviceContext()->PSSetConstantBuffers(1, 1, cb_ps_SpotLight.GetAddressOf());
	m_DeviceResources.GetDeviceContext()->PSSetConstantBuffers(2, 1, cb_ps_material.GetAddressOf());

	m_DeviceResources.GetDeviceContext()->VSSetConstantBuffers(0, 1, cb_vs_vertexshader.GetAddressOf());
	m_DeviceResources.GetDeviceContext()->VSSetConstantBuffers(1, 1, cb_vs_BoneInfo.GetAddressOf());

	m_DeviceResources.GetDeviceContext()->ClearRenderTargetView(m_DeviceResources.GetBaseRenderTargetView(), m_BackgroundColor);
	for (int i = 0; i < DeviceResources::RenderTargetCount; i++) {
		m_DeviceResources.GetDeviceContext()->ClearRenderTargetView(m_DeviceResources.GetRenderTargetView(i), m_BackgroundColor);
	}
	m_DeviceResources.GetDeviceContext()->ClearDepthStencilView(m_DeviceResources.GetBaseDepthStencilView(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	m_DeviceResources.GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY::D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	m_DeviceResources.GetDeviceContext()->RSSetState(m_DeviceResources.GetRasterizerState());
	m_DeviceResources.GetDeviceContext()->OMSetDepthStencilState(m_DeviceResources.GetBaseDepthStencilState(), 0);
	m_DeviceResources.GetDeviceContext()->OMSetBlendState(m_DeviceResources.GetBlendState(), m_BlendFactors, 0xFFFFFFFF);
	auto samplerClamp = m_DeviceResources.GetCommonState()->PointClamp();
	auto samplerWrap = m_DeviceResources.GetCommonState()->LinearWrap();
	m_DeviceResources.GetDeviceContext()->PSSetSamplers(0, 1, &samplerClamp);
	m_DeviceResources.GetDeviceContext()->PSSetSamplers(1, 1, &samplerWrap);
}

void Graphics::RenderModels()
{
	auto mainCam = Engine::Get().GetCurrentScene().GetMainCam();
	m_TargetViewProjectionMatrix = mainCam->GetViewProjectionMatrix();
	m_CullFrustum = mainCam->GetViewFrustum();

	m_DeviceResources.GetDeviceContext()->OMSetRenderTargets(
		DeviceResources::DeferredRenderChannelCount, 
		m_DeviceResources.GetRTVaddress(0), 
		m_DeviceResources.GetBaseDepthStencilView());

	m_DrawFlag = DrawFlag::All;
	static auto drawFunc = std::bind(&Graphics::Render, this, std::placeholders::_1);
	Core::Pool<RenderInfo>::GetInstance().ForEach(drawFunc);

	m_DeviceResources.GetDeviceContext()->OMSetRenderTargets(
		DeviceResources::DeferredRenderChannelCount,
		m_NullRtv,
		NULL);
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
	const std::shared_ptr<MeshBase>& mesh,
	const DirectX::XMMATRIX & worldMat, 
	const DirectX::XMMATRIX & wvpMat)
{
	cb_vs_vertexshader.data.wvpMatrix = mesh->GetTransformMatrix() * wvpMat; 
	cb_vs_vertexshader.data.worldMatrix = mesh->GetTransformMatrix() * worldMat; 
	cb_vs_vertexshader.ApplyChanges();

	UINT offset = 0;
	m_DeviceResources.GetDeviceContext()->IASetVertexBuffers(0, 1, mesh->GetVertexBufferAddr(), mesh->GetVertexBufferStridePtr(), &offset);
	m_DeviceResources.GetDeviceContext()->IASetIndexBuffer(mesh->GetIndexBuffer().Get(), DXGI_FORMAT::DXGI_FORMAT_R32_UINT, 0);
	m_DeviceResources.GetDeviceContext()->DrawIndexed(mesh->GetIndexBuffer().IndexCount(), 0, 0);
}

void Graphics::DebugDraw(const std::shared_ptr<RenderInfo>& renderer)
{
	auto * batch = m_DeviceResources.GetPrimitiveBatch();

	if (renderer->m_Model) {
		for (auto& mesh : renderer->m_Model->GetMeshes()) {
			GUI::Draw(batch, Math::GetGlobalBoundingBox(mesh->GetLocalAABB(), renderer->GetGameObject()->GetTransform()));
		}
	}
}

void Graphics::ApplyMaterialProperties(const std::shared_ptr<Material>& material)
{
	if (m_DrawFlag & DrawFlag::Apply_MaterialVertexShader) {
		m_DeviceResources.GetDeviceContext()->IASetInputLayout(material->Vshader->GetInputLayout());
		m_DeviceResources.GetDeviceContext()->VSSetShader(material->Vshader->GetShader(), NULL, 0);
	}
	if (m_DrawFlag & DrawFlag::Apply_MaterialPixelShader) {
		m_DeviceResources.GetDeviceContext()->PSSetShader(material->Pshader->GetShader(), NULL, 0);
	}
	if (m_DrawFlag & DrawFlag::Apply_MaterialTexture){
		if (material->MainTexture &&
			material->MainTexture->GetType() == aiTextureType::aiTextureType_DIFFUSE) {
			m_DeviceResources.GetDeviceContext()->PSSetShaderResources(0, 1, material->MainTexture->GetTextureResourceViewAddress());
		}
		else {
			m_DeviceResources.GetDeviceContext()->PSSetShaderResources(0, 1, m_DefaultTexture.lock()->GetTextureResourceViewAddress());
		}
		if (material->NormalMap) {
			m_DeviceResources.GetDeviceContext()->PSSetShaderResources(1, 1, material->NormalMap->GetTextureResourceViewAddress());
		}
		else {
			m_DeviceResources.GetDeviceContext()->PSSetShaderResources(1, 1, m_DefaultTexture.lock()->GetTextureResourceViewAddress());
		}
	}
	cb_ps_material.data.color = material->Color;
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
	if (m_DrawFlag & DrawFlag::Apply_ViewFrustumCulling == 0) return true;

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

	m_DeviceResources.GetDeviceContext()->OMSetRenderTargets(1, m_DeviceResources.GetRTVaddress(DeviceResources::DeferredRenderChannelCount), NULL);

	m_DeviceResources.GetDeviceContext()->VSSetShader(m_PostProcesVshader->GetShader(), NULL, 0);
	m_DeviceResources.GetDeviceContext()->PSSetShader(m_PostProcesPshader->GetShader(), NULL, 0);

	m_DeviceResources.GetDeviceContext()->PSSetShaderResources(0, 1, m_DeviceResources.GetRenderTargetSrvAddress(0));	//pos
	m_DeviceResources.GetDeviceContext()->PSSetShaderResources(1, 1, m_DeviceResources.GetRenderTargetSrvAddress(1));	//normal
	m_DeviceResources.GetDeviceContext()->PSSetShaderResources(2, 1, m_DeviceResources.GetRenderTargetSrvAddress(2));	//color
	m_DeviceResources.GetDeviceContext()->PSSetShaderResources(3, 1, l->GetShadowMapShaderResourceViewAddr());	//shadowmap
	m_DeviceResources.GetDeviceContext()->PSSetShaderResources(4, 1, m_DeviceResources.GetRenderTargetSrvAddress(4));	//depth
	m_DeviceResources.GetDeviceContext()->PSSetShaderResources(8, 1, m_SSAOrefTexture.lock()->GetTextureResourceViewAddress());	//SSAO random map
	m_DeviceResources.GetDeviceContext()->PSSetShaderResources(9, 1, m_Skybox->GetCubeMapView());	//skybox

	//m_DeviceResources.GetDeviceContext()->PSSetShaderResources(4, 1, m_DeviceResources.GetBaseDepthStencilSrvAddress());	//depth
	//m_DeviceResources.GetDeviceContext()->PSSetShaderResources(4, 1, m_DeviceResources.GetRenderTargetSrvAddress(3));	//blur 계산이후



	auto& mesh = m_PostProcesWindowModel->GetMeshes()[0];
	UINT offset = 0;
	m_DeviceResources.GetDeviceContext()->IASetVertexBuffers(0, 1, mesh->GetVertexBufferAddr(), mesh->GetVertexBufferStridePtr(), &offset);
	m_DeviceResources.GetDeviceContext()->IASetIndexBuffer(mesh->GetIndexBuffer().Get(), DXGI_FORMAT::DXGI_FORMAT_R32_UINT, 0);
	m_DeviceResources.GetDeviceContext()->DrawIndexed(mesh->GetIndexBuffer().IndexCount(), 0, 0);

	m_DeviceResources.GetDeviceContext()->PSSetShaderResources(0, DeviceResources::DeferredRenderChannelCount + 1, m_NullSrv);
	m_DeviceResources.GetDeviceContext()->PSSetShaderResources(9, 1, m_NullSrv);
	m_DeviceResources.GetDeviceContext()->OMSetRenderTargets(DeviceResources::DeferredRenderChannelCount, m_NullRtv, NULL);
}

void Graphics::DrawSkybox()
{
	m_DeviceResources.GetDeviceContext()->OMSetRenderTargets(
		DeviceResources::DeferredRenderChannelCount,
		m_DeviceResources.GetRTVaddress(0),
		m_DeviceResources.GetBaseDepthStencilView());

	m_DeviceResources.GetDeviceContext()->IASetInputLayout(m_Skybox->GetVertexShader()->GetInputLayout());
	m_DeviceResources.GetDeviceContext()->VSSetShader(m_Skybox->GetVertexShader()->GetShader(), NULL, 0);
	m_DeviceResources.GetDeviceContext()->PSSetShader(m_Skybox->GetPixelShader()->GetShader(), NULL, 0);
	m_DeviceResources.GetDeviceContext()->PSSetShaderResources(1, 1, m_Skybox->GetCubeMapView());

	m_DeviceResources.GetDeviceContext()->RSSetState(m_Skybox->GetRasterizerState());
	m_DeviceResources.GetDeviceContext()->OMSetDepthStencilState(m_Skybox->GetDepthStencilState(), 0);

	m_DrawFlag = DrawFlag::None;

	auto mainCam = Engine::Get().GetCurrentScene().GetMainCam();
	auto worldMat = DirectX::XMMatrixTranslationFromVector(mainCam->GetTransform().positionVec);
	auto wvpMat = worldMat * mainCam->GetViewProjectionMatrix();
	for (auto & mesh : m_Skybox->GetModel()->GetMeshes()) {
		DrawMesh(mesh, worldMat, wvpMat);
	}

	m_DeviceResources.GetDeviceContext()->RSSetState(m_DeviceResources.GetRasterizerState());
	m_DeviceResources.GetDeviceContext()->OMSetDepthStencilState(m_DeviceResources.GetBaseDepthStencilState(), 0);

	m_DeviceResources.GetDeviceContext()->OMSetRenderTargets(
		DeviceResources::DeferredRenderChannelCount,
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

	auto& dr = Engine::Get().GetGraphics().GetDeviceResources();
	
	dr.GetDeviceContext()->ClearDepthStencilView(dr.GetSubDepthStencilView(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	dr.GetDeviceContext()->ClearRenderTargetView(spotLight->GetShadowMapRenderTargetView(), m_BackgroundColor);
	dr.GetDeviceContext()->OMSetRenderTargets(1, spotLight->GetShadowMapRenderTargetViewAddr(), dr.GetSubDepthStencilView());
	dr.GetDeviceContext()->PSSetShader(m_ShadowMapPshader->GetShader(), NULL, 0);

	m_DrawFlag = 
		DrawFlag::Apply_MaterialVertexShader | 
		DrawFlag::Apply_SkinnedMeshBone;

	static auto drawFunc = std::bind(&Graphics::Render, this, std::placeholders::_1);
	Core::Pool<RenderInfo>::GetInstance().ForEach(drawFunc);

	dr.GetDeviceContext()->OMSetRenderTargets(1, m_NullRtv, NULL);
}

void Graphics::DrawGui()
{
	auto& dr = m_DeviceResources;
	auto light = Core::Find<GameObject>("Light")->GetComponent<SpotLight>();

	m_DeviceResources.GetDeviceContext()->OMSetRenderTargets(1, m_DeviceResources.GetBaseRTVaddress(), NULL);

	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
	GUI::DrawEditorUI(m_DeviceResources.GetRenderTargetSrv(DeviceResources::DeferredRenderChannelCount));
	
	ImGuiIO& io = ImGui::GetIO();
	ImVec2 scene_size = ImVec2(io.DisplaySize.x * 0.2f, io.DisplaySize.y * 0.2f);

	

	ImGui::Begin("Texture Resources");
	Core::Pool<Texture>::GetInstance().ForEach(GUI::DrawTexture);
	ImGui::End();

	ImGui::Begin("Blur Test");
	ImGui::DragFloat("ThresHold", &cb_cs_DownSample.data.threshold, 0.01f, 0.0f, 10.0f);
	ImGui::Image(dr.GetRenderTargetSrv(3), scene_size);
	ImGui::Image(dr.GetRenderTargetSrv(5), scene_size);
	ImGui::Image(dr.GetRenderTargetSrv(6), scene_size);
	ImGui::Image(dr.GetBaseDepthStencilSrv(), scene_size);
	ImGui::Image(dr.GetSubDepthStencilSrv(), scene_size);

	ImGui::End();

	GUI::DrawDeferredChannelImage();

	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

	m_DeviceResources.GetDeviceContext()->OMSetRenderTargets(1, m_NullRtv, NULL);
}

void Graphics::DrawGuiDebug()
{
	auto * context = m_DeviceResources.GetDeviceContext();
	auto * states = m_DeviceResources.GetCommonState();
	auto * effect = m_DeviceResources.GetBasicEffect();
	auto * batch = m_DeviceResources.GetPrimitiveBatch();
	auto& camera = *Engine::Get().GetCurrentScene().GetMainCam();

	effect->SetVertexColorEnabled(true);
	effect->SetView(camera.GetViewMatrix());
	effect->SetProjection(camera.GetProjectionMatrix());

	context->OMSetBlendState(states->Opaque(), nullptr, 0xFFFFFFFF);
	context->OMSetDepthStencilState(states->DepthNone(), 0);
	context->RSSetState(states->CullNone());

	effect->Apply(context);

	context->IASetInputLayout(m_DeviceResources.GetDebugInputLayout());

	m_DeviceResources.GetDeviceContext()->OMSetRenderTargets(
		1,
		m_DeviceResources.GetRTVaddress(DeviceResources::DeferredRenderChannelCount),
		m_DeviceResources.GetBaseDepthStencilView());

	batch->Begin();

	static auto drawFunc = std::bind(&Graphics::DebugDraw, this, std::placeholders::_1);
	Core::Pool<RenderInfo>::GetInstance().ForEach(drawFunc);

	batch->End();

	m_DeviceResources.GetDeviceContext()->OMSetRenderTargets(
		DeviceResources::DeferredRenderChannelCount,
		m_NullRtv,
		NULL);
}

void Graphics::ComputeShdaderTest()
{
	static auto csBlur = Core::Find<ComputeShader>("Blur");
	static auto csDownsample = Core::Find<ComputeShader>("ThresholdDownSample");
	auto light = Core::Find<GameObject>("Light")->GetComponent<SpotLight>();
	auto& dr = m_DeviceResources;
	
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
		m_DeviceResources.GetDeviceContext()->CSSetShader(csDownsample->GetShader(), 0, 0);
		m_DeviceResources.GetDeviceContext()->CSSetConstantBuffers(0, 1, cb_cs_DownSample.GetAddressOf());
		m_DeviceResources.GetDeviceContext()->CSSetShaderResources(0, 1, dr.GetRenderTargetSrvAddress(3));
		m_DeviceResources.GetDeviceContext()->CSSetUnorderedAccessViews(0, 1, dr.GetRenderTargetUavAddr(5), 0);

		m_DeviceResources.GetDeviceContext()->Dispatch(windowWidth / 16, windowHeight / 16, 1);

		m_DeviceResources.GetDeviceContext()->CSSetUnorderedAccessViews(0, 1, m_NullUav, 0);
		m_DeviceResources.GetDeviceContext()->CSSetShaderResources(0, 1, m_NullSrv);
	}
	{
		m_DeviceResources.GetDeviceContext()->CSSetShader(csBlur->GetShader(), 0, 0);
		ID3D11ShaderResourceView* csSRVs[2] = { 
			dr.GetRenderTargetSrv(5), 
			dr.GetRenderTargetSrv(6) };
		ID3D11UnorderedAccessView* csUAVs[2] = {
			dr.GetRenderTargetUav(6),
			dr.GetRenderTargetUav(5) };

		for (UINT direction = 0; direction < 2; ++direction)
		{
			cb_cs_ThresholdBlur.data.direction = direction;
			cb_cs_ThresholdBlur.ApplyChanges();
			m_DeviceResources.GetDeviceContext()->CSSetConstantBuffers(0, 1, cb_cs_ThresholdBlur.GetAddressOf());
			m_DeviceResources.GetDeviceContext()->CSSetShaderResources(0, 1, &csSRVs[direction]);
			m_DeviceResources.GetDeviceContext()->CSSetUnorderedAccessViews(0, 1, &csUAVs[direction], 0);

			m_DeviceResources.GetDeviceContext()->Dispatch(windowWidth / 16, windowHeight / 16, 1);

			m_DeviceResources.GetDeviceContext()->CSSetShaderResources(0, 1, m_NullSrv);
			m_DeviceResources.GetDeviceContext()->CSSetUnorderedAccessViews(0, 1, m_NullUav, 0);
		}
	}
}

void Graphics::RenderEnd()
{
	m_DeviceResources.GetSwapChain()->Present(1, NULL);
}