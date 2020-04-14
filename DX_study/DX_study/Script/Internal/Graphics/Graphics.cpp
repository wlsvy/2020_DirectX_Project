#include "Graphics.h"

#include "imGui/imgui.h"
#include "imGui/imgui_impl_win32.h"
#include "imGui/imgui_impl_dx11.h"
#include "Model.h"
#include "Shaders.h"
#include "Skybox.h"
#include "BaseGeometry.h"
#include "AnimationClip.h"
#include "../../Util/Time.h"
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
		this->windowWidth = width;
		this->windowHeight = height;

		ThrowIfFailed(
			m_DeviceResources.Initialize(hwnd, width, height) &&
			m_DeviceResources.InitializeRenderTarget(width, height),
			"Failed to initialize Device Resources");
		ThrowIfFailed(cb_vs_vertexshader_2d.Initialize(), "Failed to Initialize CB_VS_vertexshader_2d buffer.");
		ThrowIfFailed(cb_vs_vertexshader.Initialize(), "Failed to Initialize CB_VS_vertexshader buffer.");
		ThrowIfFailed(cb_ps_light.Initialize(), "Failed to Initialize cb_ps_light buffer.");
		ThrowIfFailed(cb_BoneInfo.Initialize(), "Failed to Initialize cb_BoneInfo buffer.");

		Importer::LoadData();
		BaseGeometry::Initialize();

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

		UI::InitImGUI(hwnd);

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

void Graphics::Draw(const std::shared_ptr<Renderable>& renderer)
{
	if (!renderer->Vshader ||
		!renderer->Pshader)
	{
		return;
	}

	m_DeviceResources.GetDeviceContext()->IASetInputLayout(renderer->Vshader->GetInputLayout());
	m_DeviceResources.GetDeviceContext()->VSSetShader(renderer->Vshader->GetShader(), NULL, 0);
	m_DeviceResources.GetDeviceContext()->PSSetShader(renderer->Pshader->GetShader(), NULL, 0);
	m_DeviceResources.GetDeviceContext()->VSSetConstantBuffers(0, 1, cb_vs_vertexshader.GetAddressOf());

	auto& worldMat = renderer->GetGameObject()->GetTransform().GetWorldMatrix();
	auto wvpMat = worldMat * Engine::Get().GetCurrentScene().GetMainCam()->GetViewProjectionMatrix();
	
	if (renderer->Model) {
		DrawModel(renderer->Model, worldMat, wvpMat);
	}
	else if (renderer->SkinnedModel &&
		renderer->Anim &&
		renderer->Anim->Clip) 
	{
		CopyMemory(cb_BoneInfo.data.boneTransform, 
			renderer->Anim->GetAnimResult().data(), 
			renderer->Anim->GetAnimResult().size() * sizeof(DirectX::XMMATRIX));
		cb_BoneInfo.ApplyChanges();

		m_DeviceResources.GetDeviceContext()->VSSetConstantBuffers(1, 1, cb_BoneInfo.GetAddressOf());
		DrawModel(renderer->SkinnedModel, worldMat, wvpMat);
	}
}

void Graphics::DrawModel(
	const std::shared_ptr<Model>& model, 
	const DirectX::XMMATRIX & worldMat, 
	const DirectX::XMMATRIX & wvpMat)
{
	for (auto& mesh : model->GetMeshes()) {
		DrawMesh(mesh, worldMat, wvpMat);
	}
}
void Graphics::DrawModel(
	const std::shared_ptr<SkinnedModel>& model,
	const DirectX::XMMATRIX & worldMat,
	const DirectX::XMMATRIX & wvpMat)
{
	for (auto& mesh : model->GetMeshes()) {
		cb_vs_vertexshader.data.wvpMatrix = mesh.GetTransformMatrix() * wvpMat; //Calculate World-View-Projection Matrix
		cb_vs_vertexshader.data.worldMatrix = mesh.GetTransformMatrix() * worldMat; //Calculate World Matrix
		cb_vs_vertexshader.ApplyChanges();

		for (auto& texture : mesh.GetTextures()) {
			if (texture.GetType() == aiTextureType::aiTextureType_DIFFUSE) {
				m_DeviceResources.GetDeviceContext()->PSSetShaderResources(0, 1, texture.GetTextureResourceViewAddress());
				break;
			}
		}

		UINT offset = 0;
		m_DeviceResources.GetDeviceContext()->IASetVertexBuffers(0, 1, mesh.GetVertexBuffer().GetAddressOf(), mesh.GetVertexBuffer().StridePtr(), &offset);
		m_DeviceResources.GetDeviceContext()->IASetIndexBuffer(mesh.GetIndexBuffer().Get(), DXGI_FORMAT::DXGI_FORMAT_R32_UINT, 0);
		m_DeviceResources.GetDeviceContext()->DrawIndexed(mesh.GetIndexBuffer().IndexCount(), 0, 0);
	}
}
void Graphics::DrawMesh(
	const Mesh & mesh,
	const DirectX::XMMATRIX & worldMat, 
	const DirectX::XMMATRIX & wvpMat)
{
	cb_vs_vertexshader.data.wvpMatrix = mesh.GetTransformMatrix() * wvpMat; //Calculate World-View-Projection Matrix
	cb_vs_vertexshader.data.worldMatrix = mesh.GetTransformMatrix() * worldMat; //Calculate World Matrix
	cb_vs_vertexshader.ApplyChanges();

	for (auto& texture : mesh.GetTextures()) {
		if (texture.GetType() == aiTextureType::aiTextureType_DIFFUSE) {
			m_DeviceResources.GetDeviceContext()->PSSetShaderResources(0, 1, texture.GetTextureResourceViewAddress());
			break;
		}
	}

	UINT offset = 0;
	m_DeviceResources.GetDeviceContext()->IASetVertexBuffers(0, 1, mesh.GetVertexBuffer().GetAddressOf(), mesh.GetVertexBuffer().StridePtr(), &offset);
	m_DeviceResources.GetDeviceContext()->IASetIndexBuffer(mesh.GetIndexBuffer().Get(), DXGI_FORMAT::DXGI_FORMAT_R32_UINT, 0);
	m_DeviceResources.GetDeviceContext()->DrawIndexed(mesh.GetIndexBuffer().IndexCount(), 0, 0);
}

void Graphics::PostProcess()
{
	m_DeviceResources.GetDeviceContext()->VSSetShader(m_PostProcesVshader->GetShader(), NULL, 0);
	m_DeviceResources.GetDeviceContext()->PSSetShader(m_PostProcesPshader->GetShader(), NULL, 0);

	m_DeviceResources.GetDeviceContext()->PSSetShaderResources(0, 1, m_DeviceResources.GetRenderTargetSrvAddress(0));
	m_DeviceResources.GetDeviceContext()->PSSetShaderResources(1, 1, m_DeviceResources.GetRenderTargetSrvAddress(1));
	m_DeviceResources.GetDeviceContext()->PSSetShaderResources(2, 1, m_DeviceResources.GetRenderTargetSrvAddress(2));

	auto& vertexBuffer = m_PostProcesWindowModel->GetMeshes()[0].GetVertexBuffer();
	auto& indexBuffer = m_PostProcesWindowModel->GetMeshes()[0].GetIndexBuffer();
	UINT offset = 0;
	m_DeviceResources.GetDeviceContext()->IASetVertexBuffers(0, 1, vertexBuffer.GetAddressOf(), vertexBuffer.StridePtr(), &offset);
	m_DeviceResources.GetDeviceContext()->IASetIndexBuffer(indexBuffer.Get(), DXGI_FORMAT::DXGI_FORMAT_R32_UINT, 0);
	m_DeviceResources.GetDeviceContext()->DrawIndexed(indexBuffer.IndexCount(), 0, 0);
}

void Graphics::DrawUI()
{
	
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
	UI::DrawEditorUI(m_DeviceResources.GetRenderTargetSrv(3));
	
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

	
	UI::DrawDeferredChannelImage();
	Engine::Get().GetCurrentScene().OnGui();

	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
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
	auto worldMat = DirectX::XMMatrixTranslationFromVector(mainCam->GetTransform().GetPositionVector());
	auto wvpMat = worldMat * mainCam->GetViewProjectionMatrix();
	DrawModel(m_Skybox->GetModel(), worldMat, wvpMat);

	m_DeviceResources.GetDeviceContext()->RSSetState(m_DeviceResources.GetRasterizerState());
	m_DeviceResources.GetDeviceContext()->OMSetDepthStencilState(m_DeviceResources.GetBaseDepthStencilState(), 0);
}

void Graphics::SetRenderTarget(ID3D11RenderTargetView * const * rtv, int bufferCount)
{
	m_DeviceResources.GetDeviceContext()->OMSetRenderTargets(bufferCount, rtv, m_DeviceResources.GetBaseDepthStencilView());
}

void Graphics::SwapBuffer()
{
	m_DeviceResources.GetSwapChain()->Present(1, NULL);
}