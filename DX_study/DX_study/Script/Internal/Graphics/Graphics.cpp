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
#include "../../GameObject/Light.h"
#include "../../GameObject/Camera.h"


using DirectX::operator*;

bool Graphics::Initialize(HWND hwnd, int width, int height) {
	try {
		windowWidth = width;
		windowHeight = height;

		ThrowIfFailed(m_DeviceResources.Initialize(hwnd, width, height), "Failed to initialize Device Resources");
		ThrowIfFailed(m_DeviceResources.InitializeRenderTarget(width, height), "Failed to initialize Device Resources RenderTarget");
		ThrowIfFailed(cb_vs_vertexshader_2d.Initialize(), "Failed to Initialize CB_VS_vertexshader_2d buffer.");
		ThrowIfFailed(cb_vs_vertexshader.Initialize(), "Failed to Initialize CB_VS_vertexshader buffer.");
		ThrowIfFailed(cb_vs_BoneInfo.Initialize(), "Failed to Initialize cb_vs_BoneInfo buffer.");
		ThrowIfFailed(cb_ps_light.Initialize(), "Failed to Initialize cb_ps_light buffer.");
		ThrowIfFailed(cb_ps_material.Initialize(), "Failed to Initialize cb_ps_material buffer.");

		ThrowIfFailed(Importer::LoadBaseResources(), "Failed to LoadBaseResources.");
		ThrowIfFailed(ProcessMaterialTable(), "Failed to ProcessMaterialTable.");
		ThrowIfFailed(BaseGeometry::Initialize(), "Failed to Initialize BaseGeometry.");
		ThrowIfFailed(Importer::LoadModelResources(), "Failed to LoadModelResources.");

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

		m_PostProcesWindowModel = Pool::Find<Model>("Plane");
		m_PostProcesVshader = Pool::Find<VertexShader>("vertexshader_WindowPlane");
		m_PostProcesPshader = Pool::Find<PixelShader>("pixelshader_deferredLight");

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
			auto material = Pool::CreateInstance<SharedMaterial>(table["Name"][i]);

			material->Vshader = Pool::Find<VertexShader>(table["VertexShader"][i]);
			material->Pshader = Pool::Find<PixelShader>(table["PixelShader"][i]);
			material->MainTexture = Pool::Find<Texture>(table["Texture"][i]);

			auto splitted = Importer::SplitString(table["Color"][i], '/');
			material->Color.RGBA[0] = std::stoi(splitted[0]);
			material->Color.RGBA[1] = std::stoi(splitted[1]);
			material->Color.RGBA[2] = std::stoi(splitted[2]);
			material->Color.RGBA[3] = std::stoi(splitted[3]);
		}
		return true;
	}
	catch (CustomException &e) {
		ErrorLogger::Log(e);
		return false;
	}
}

void Graphics::RenderFrame()
{
	static auto light = std::dynamic_pointer_cast<Light>(Pool::Find<GameObject>("Light"));
	cb_ps_light.data.color = light->color;
	cb_ps_light.data.strength = light->strength;
	cb_ps_light.data.position = light->GetTransform().position;
	cb_ps_light.data.attenuation = light->attenuation;
	DirectX::XMStoreFloat3(&cb_ps_light.data.forwardVector, light->GetTransform().GetForwardVector());
	cb_ps_light.data.spotAngle = light->spotAngle;
	cb_ps_light.data.range = light->range;
	cb_ps_light.ApplyChanges();
	m_DeviceResources.GetDeviceContext()->PSSetConstantBuffers(0, 1, cb_ps_light.GetAddressOf());
	m_DeviceResources.GetDeviceContext()->PSSetConstantBuffers(1, 1, cb_ps_material.GetAddressOf());
	m_DeviceResources.GetDeviceContext()->VSSetConstantBuffers(1, 1, cb_vs_BoneInfo.GetAddressOf());
	m_DeviceResources.GetDeviceContext()->VSSetConstantBuffers(0, 1, cb_vs_vertexshader.GetAddressOf());

	m_DeviceResources.GetDeviceContext()->ClearRenderTargetView(m_DeviceResources.GetBaseRenderTargetView(), m_BackgroundColor);
	for (int i = 0; i < DeviceResources::RenderTargetCount; i++) {
		m_DeviceResources.GetDeviceContext()->ClearRenderTargetView(m_DeviceResources.GetRenderTargetView(i), m_BackgroundColor);
	}
	m_DeviceResources.GetDeviceContext()->ClearDepthStencilView(m_DeviceResources.GetBaseDepthStencilView(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	m_DeviceResources.GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY::D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	m_DeviceResources.GetDeviceContext()->RSSetState(m_DeviceResources.GetRasterizerState());
	m_DeviceResources.GetDeviceContext()->OMSetDepthStencilState(m_DeviceResources.GetBaseDepthStencilState(), 0);
	m_DeviceResources.GetDeviceContext()->OMSetBlendState(m_DeviceResources.GetBlendState(), m_BlendFactors, 0xFFFFFFFF);
	m_DeviceResources.GetDeviceContext()->PSSetSamplers(0, 1, m_DeviceResources.GetSamplerStateAddr());

	
}

void Graphics::PushToRenderQueue(const std::shared_ptr<RenderInfo>& renderer)
{
	auto & renderables = renderer->GetRenerables();
	renderer->m_IsVisible = false;

	if (renderables.size() == 0) {
		return;
	}

	auto& tf = renderer->m_GameObject->GetTransform();
	auto mainCam = Engine::Get().GetCurrentScene().GetMainCam();
	
	bool isVisible = true;
	for (auto & r : renderables) {
		auto globalAABB = Math::GetGlobalBoundingBox(r.GetMesh()->GetLocalAABB(), tf);
		auto containment = mainCam->GetViewFrustum().Contains(globalAABB);
		isVisible &= containment != DirectX::DISJOINT;
		
	}
	renderer->m_IsVisible = isVisible;

	if (!isVisible) {
		return;
	}

	if (renderer->Anim &&
		renderer->Anim->GetClip())
	{
		CopyMemory(cb_vs_BoneInfo.data.boneTransform,
			renderer->Anim->GetAnimResult().data(),
			renderer->Anim->GetAnimResult().size() * sizeof(DirectX::XMMATRIX));
		cb_vs_BoneInfo.ApplyChanges();
	}

	auto& worldMat = tf.GetWorldMatrix();
	auto wvpMat = worldMat * mainCam->GetViewProjectionMatrix();
	
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
	cb_vs_vertexshader.data.wvpMatrix = mesh->GetTransformMatrix() * wvpMat; //Calculate World-View-Projection Matrix
	cb_vs_vertexshader.data.worldMatrix = mesh->GetTransformMatrix() * worldMat; //Calculate World Matrix
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
	m_DeviceResources.GetDeviceContext()->IASetInputLayout(material->Vshader->GetInputLayout());
	m_DeviceResources.GetDeviceContext()->VSSetShader(material->Vshader->GetShader(), NULL, 0);
	m_DeviceResources.GetDeviceContext()->PSSetShader(material->Pshader->GetShader(), NULL, 0);
	if (material->MainTexture->GetType() == aiTextureType::aiTextureType_DIFFUSE) {
		m_DeviceResources.GetDeviceContext()->PSSetShaderResources(0, 1, material->MainTexture->GetTextureResourceViewAddress());
	}
	cb_ps_material.data.color = material->Color;
	cb_ps_material.ApplyChanges();
}

void Graphics::PostProcess()
{
	m_DeviceResources.GetDeviceContext()->VSSetShader(m_PostProcesVshader->GetShader(), NULL, 0);
	m_DeviceResources.GetDeviceContext()->PSSetShader(m_PostProcesPshader->GetShader(), NULL, 0);

	m_DeviceResources.GetDeviceContext()->PSSetShaderResources(0, 1, m_DeviceResources.GetRenderTargetSrvAddress(0));
	m_DeviceResources.GetDeviceContext()->PSSetShaderResources(1, 1, m_DeviceResources.GetRenderTargetSrvAddress(1));
	m_DeviceResources.GetDeviceContext()->PSSetShaderResources(2, 1, m_DeviceResources.GetRenderTargetSrvAddress(2));

	auto& mesh = m_PostProcesWindowModel->GetMeshes()[0];
	UINT offset = 0;
	m_DeviceResources.GetDeviceContext()->IASetVertexBuffers(0, 1, mesh->GetVertexBufferAddr(), mesh->GetVertexBufferStridePtr(), &offset);
	m_DeviceResources.GetDeviceContext()->IASetIndexBuffer(mesh->GetIndexBuffer().Get(), DXGI_FORMAT::DXGI_FORMAT_R32_UINT, 0);
	m_DeviceResources.GetDeviceContext()->DrawIndexed(mesh->GetIndexBuffer().IndexCount(), 0, 0);

	ID3D11ShaderResourceView * nullSrv[3] = { NULL, NULL, NULL };
	m_DeviceResources.GetDeviceContext()->PSSetShaderResources(0, 3, nullSrv);
}

void Graphics::DrawSkybox()
{
	m_DeviceResources.GetDeviceContext()->IASetInputLayout(m_Skybox->GetVertexShader()->GetInputLayout());
	m_DeviceResources.GetDeviceContext()->VSSetShader(m_Skybox->GetVertexShader()->GetShader(), NULL, 0);
	m_DeviceResources.GetDeviceContext()->PSSetShader(m_Skybox->GetPixelShader()->GetShader(), NULL, 0);
	m_DeviceResources.GetDeviceContext()->GSSetShader(NULL, NULL, 0);
	m_DeviceResources.GetDeviceContext()->RSSetState(m_Skybox->GetRasterizerState());
	m_DeviceResources.GetDeviceContext()->OMSetDepthStencilState(m_Skybox->GetDepthStencilState(), 0);
	m_DeviceResources.GetDeviceContext()->PSSetShaderResources(1, 1, m_Skybox->GetCubeMapView());

	auto mainCam = Engine::Get().GetCurrentScene().GetMainCam();
	auto worldMat = DirectX::XMMatrixTranslationFromVector(mainCam->GetTransform().positionVec);
	auto wvpMat = worldMat * mainCam->GetViewProjectionMatrix();
	for (auto & mesh : m_Skybox->GetModel()->GetMeshes()) {
		DrawMesh(mesh, worldMat, wvpMat);
	}

	m_DeviceResources.GetDeviceContext()->RSSetState(m_DeviceResources.GetRasterizerState());
	m_DeviceResources.GetDeviceContext()->OMSetDepthStencilState(m_DeviceResources.GetBaseDepthStencilState(), 0);
}

void Graphics::DrawGui()
{
	
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
	GUI::DrawEditorUI(m_DeviceResources.GetRenderTargetSrv(3));
	
	static auto light = std::dynamic_pointer_cast<Light>(Pool::Find<GameObject>("Light"));
	ImGui::Begin("Light Controls");
	ImGui::DragFloat3("Ambient Light Color", &cb_ps_light.data.ambientLightColor.x, 0.01f, 0.0f, 1.0f);
	ImGui::DragFloat("Ambient Light Strenght", &cb_ps_light.data.ambientLightStrength, 0.01f, 0.0f, 1.0f);
	ImGui::NewLine();
	ImGui::DragFloat3("Dynamic Light Color", &light->color.x, 0.01f, 0.0f, 10.0f);
	ImGui::DragFloat("Dynamic Light Strength", &light->strength, 0.01f, 0.0f, 10.0f);
	ImGui::DragFloat3("Dynamic Light Attenuation", &light->attenuation.x, 0.01f, 0.1f, 10.0f);
	ImGui::DragFloat3("Dir", &light->GetTransform().rotation.x, 0.1f, 0.0f, 90.0f);
	ImGui::DragFloat("spotangle", &light->spotAngle, 0.01f, 0.0f, 180.0f);
	ImGui::DragFloat("Range", &light->range, 0.1f, 0.1f, 1000.0f);
	ImGui::End();

	ImGui::Begin("Texture Resources");
	ImGuiIO& io = ImGui::GetIO();
	ImVec2 scene_size = ImVec2(io.DisplaySize.x * 0.2f, io.DisplaySize.y * 0.2f);
	Pool::ObjectPool<Texture>::GetInstance().ForEach(GUI::DrawTexture);
	ImGui::End();

	GUI::DrawDeferredChannelImage();

	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
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

	batch->Begin();

	static auto drawFunc = std::bind(&Graphics::DebugDraw, this, std::placeholders::_1);
	Pool::ObjectPool<RenderInfo>::GetInstance().ForEach(drawFunc);
	batch->End();

}

void Graphics::SetRenderTarget(ID3D11RenderTargetView * const * rtv, int bufferCount)
{
	ID3D11RenderTargetView * nullRTV[3] = { NULL, NULL, NULL };
	m_DeviceResources.GetDeviceContext()->OMSetRenderTargets(3, nullRTV, NULL);
	m_DeviceResources.GetDeviceContext()->OMSetRenderTargets(bufferCount, rtv, m_DeviceResources.GetBaseDepthStencilView());
}

void Graphics::SwapBuffer()
{
	m_DeviceResources.GetSwapChain()->Present(1, NULL);
}