#include "ShadowMap.h"
#include "../Engine/AdapterReader.h"
#include "../../Util/ErrorLogger.h"
#include "../Graphics/imGui/imgui.h"
#include "../Core/InternalHelper.h"
#include "Graphics.h"
#include "../Engine/Engine.h"
#include "../../GameObject/Camera.h"
#include "../../GameObject/Light.h"
#include "../Core/Scene.h"
#include "Shaders.h"
#include "../../Component/Animator.h"
#include "../../Component/Transform.h"
#include "../../Component/Renderable.h"
#include "Material.h"

bool ShadowMap::Initialize()
{
	try {
		D3D11_TEXTURE2D_DESC textureDesc;
		ZeroMemory(&textureDesc, sizeof(textureDesc));
		textureDesc.Width = 1280;
		textureDesc.Height = 720;
		textureDesc.MipLevels = 1;
		textureDesc.ArraySize = 1;
		textureDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
		textureDesc.SampleDesc.Count = 1;
		textureDesc.Usage = D3D11_USAGE_DEFAULT;
		textureDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
		textureDesc.CPUAccessFlags = 0;
		textureDesc.MiscFlags = 0;

		Microsoft::WRL::ComPtr<ID3D11Texture2D> shadowMapRenderTargetTexture;
		Microsoft::WRL::ComPtr<ID3D11Texture2D> shadowMapRenderTargetTexture2;
		ThrowIfFailed(
			Core::GetDevice()->CreateTexture2D(&textureDesc, NULL, shadowMapRenderTargetTexture.GetAddressOf()),
			"Failed to create renderTargetTextureArr.");
		ThrowIfFailed(
			Core::GetDevice()->CreateTexture2D(&textureDesc, NULL, shadowMapRenderTargetTexture2.GetAddressOf()),
			"Failed to create renderTargetTextureArr.");

		D3D11_RENDER_TARGET_VIEW_DESC renderTargetViewDesc;
		renderTargetViewDesc.Format = textureDesc.Format;
		renderTargetViewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
		renderTargetViewDesc.Texture2D.MipSlice = 0;
		ThrowIfFailed(
			Core::GetDevice()->CreateRenderTargetView(shadowMapRenderTargetTexture.Get(), &renderTargetViewDesc, m_ShadowMapRenderTargetView.GetAddressOf()),
			"Failed to create renderTargetViewArr.");
		ThrowIfFailed(
			Core::GetDevice()->CreateRenderTargetView(shadowMapRenderTargetTexture2.Get(), &renderTargetViewDesc, m_ResultRenderTargetView.GetAddressOf()),
			"Failed to create renderTargetViewArr.");

		D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc;
		shaderResourceViewDesc.Format = textureDesc.Format;
		shaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		shaderResourceViewDesc.Texture2D.MostDetailedMip = 0;
		shaderResourceViewDesc.Texture2D.MipLevels = 1;
		ThrowIfFailed(
			Core::GetDevice()->CreateShaderResourceView(shadowMapRenderTargetTexture.Get(), &shaderResourceViewDesc, m_ShadowMapShaderResourceView.GetAddressOf()),
			"Failed to create shaderResourceViewArr.");
		ThrowIfFailed(
			Core::GetDevice()->CreateShaderResourceView(shadowMapRenderTargetTexture2.Get(), &shaderResourceViewDesc, m_ResultShaderResourceView.GetAddressOf()),
			"Failed to create shaderResourceViewArr.");

		//Describe our Depth/Stencil Buffer + 생성자 활용해서 코드 줄이기(애초에 default 패러미터 값은 안 건드려도 됨)
		CD3D11_TEXTURE2D_DESC depthStencilDesc(DXGI_FORMAT_D24_UNORM_S8_UINT, 1280,	720);
		depthStencilDesc.MipLevels = 1;
		depthStencilDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;

		Microsoft::WRL::ComPtr<ID3D11Texture2D> depthStencilBuffer;
		HRESULT hr = Core::GetDevice()->CreateTexture2D(&depthStencilDesc, NULL, depthStencilBuffer.GetAddressOf());
		ThrowIfFailed(hr, "Failed to create depth stencil buffer.");

		hr = Core::GetDevice()->CreateDepthStencilView(depthStencilBuffer.Get(), NULL, this->m_DepthStencilView.GetAddressOf());
		ThrowIfFailed(hr, "Failed to create depth stencil view.");
	}
	catch (CustomException & exception) {
		ErrorLogger::Log(exception);
		return false;
	}
	return true;
}

void RenerModels(const std::shared_ptr<RenderInfo> & renderer) {
	//auto mainCam = Engine::Get().GetCurrentScene().GetMainCam();
	auto& dr = Engine::Get().GetGraphics().GetDeviceResources();
	auto& tf = renderer->GetGameObject()->GetTransform();
	if (renderer->GetGameObject()->Name == "Light") return;

	if (renderer->Anim &&
		renderer->Anim->GetClip())
	{
		CopyMemory(Core::GetCbBoneData().data.boneTransform,
			renderer->Anim->GetAnimResult().data(),
			renderer->Anim->GetAnimResult().size() * sizeof(DirectX::XMMATRIX));
		Core::GetCbBoneData().ApplyChanges();
	}

	auto& worldMat = tf.GetWorldMatrix();
	//auto vpMat = ShadowMap::GetInstance().ShadowCaster->GetLightViewProjectMat();
	auto wvpMat = worldMat * ShadowMap::GetInstance().vpMat;

	for (auto & r : renderer->GetRenerables()) {
		//ApplyMaterialProperties(r.GetMaterial());
		//m_DeviceResources.GetDeviceContext()->IASetInputLayout(vs->GetInputLayout());
		//m_DeviceResources.GetDeviceContext()->VSSetShader(vs->GetShader(), NULL, 0);
		dr.GetDeviceContext()->IASetInputLayout(r.GetMaterial()->Vshader->GetInputLayout());
		dr.GetDeviceContext()->VSSetShader(r.GetMaterial()->Vshader->GetShader(), NULL, 0);
		Engine::Get().GetGraphics().DrawMesh(r.GetMesh(), worldMat, wvpMat);
	}
}

void RenderModelResult(const std::shared_ptr<RenderInfo> & renderer) {
	//auto mainCam = Engine::Get().GetCurrentScene().GetMainCam();
	auto& dr = Engine::Get().GetGraphics().GetDeviceResources();
	auto& tf = renderer->GetGameObject()->GetTransform();
	if (renderer->GetGameObject()->Name == "Light") return;

	if (renderer->Anim &&
		renderer->Anim->GetClip())
	{
		CopyMemory(Core::GetCbBoneData().data.boneTransform,
			renderer->Anim->GetAnimResult().data(),
			renderer->Anim->GetAnimResult().size() * sizeof(DirectX::XMMATRIX));
		Core::GetCbBoneData().ApplyChanges();
	}

	auto& worldMat = tf.GetWorldMatrix();
	//auto vpMat = ShadowMap::GetInstance().ShadowCaster->GetLightViewProjectMat();
	auto wvpMat = worldMat * ShadowMap::GetInstance().vpMat;

	for (auto & r : renderer->GetRenerables()) {
		Engine::Get().GetGraphics().ApplyMaterialProperties(r.GetMaterial());
		Engine::Get().GetGraphics().DrawMesh(r.GetMesh(), worldMat, wvpMat);
	}
}

void ShadowMap::DrawShadowMap()
{
	if (!ShadowMap::GetInstance().ShadowCaster) return;

	auto ps = Core::Find<PixelShader>("pixelshader_depth");
	auto vs = Core::Find<VertexShader>("vertexshader_depth");
	auto sh = ps->GetShader();
	vpMat = ShadowMap::GetInstance().ShadowCaster->GetLightViewProjectMat();

	auto& dr = Engine::Get().GetGraphics().GetDeviceResources();
	float bc[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
	Engine::Get().GetGraphics().GetDeviceResources().GetDeviceContext()->ClearDepthStencilView(dr.GetSubDepthStencilView(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	Engine::Get().GetGraphics().GetDeviceResources().GetDeviceContext()->ClearRenderTargetView(ShadowMap::GetInstance().m_ShadowMapRenderTargetView.Get(), bc);
	Engine::Get().GetGraphics().GetDeviceResources().GetDeviceContext()->RSSetState(dr.GetRasterizerState());
	Engine::Get().GetGraphics().GetDeviceResources().GetDeviceContext()->OMSetDepthStencilState(dr.GetBaseDepthStencilState(), 0);
	Engine::Get().GetGraphics().GetDeviceResources().GetDeviceContext()->PSSetShader(sh, NULL, 0);

	ID3D11RenderTargetView * nullRTV[DeviceResources::DeferredRenderChannelCount] = { NULL, };
	Engine::Get().GetGraphics().GetDeviceResources().GetDeviceContext()->OMSetRenderTargets(DeviceResources::DeferredRenderChannelCount, nullRTV, NULL);
	Engine::Get().GetGraphics().GetDeviceResources().GetDeviceContext()->OMSetRenderTargets(1, ShadowMap::GetInstance().m_ShadowMapRenderTargetView.GetAddressOf(), dr.GetSubDepthStencilView());
	//dr.GetDeviceContext()->OMSetRenderTargets(1, ShadowMap::GetInstance().ShadowCaster->GetShadowMapRenderTargetViewAddr(), ShadowMap::GetInstance().m_DepthStencilView.Get());
	
	Core::Pool<RenderInfo>::GetInstance().ForEach(RenerModels);
	
	//dr.GetDeviceContext()->PSSetShaderResources(1, 1, ShadowMap::GetInstance().m_ShadowMapRenderTargetView.GetAddressOf());

}

void ShadowMap::DrawReult()
{
	if (!ShadowMap::GetInstance().ShadowCaster) return;

	auto mainCam = Engine::Get().GetCurrentScene().GetMainCam();
	auto ps = Core::Find<PixelShader>("pixelshader_depth");
	auto vs = Core::Find<VertexShader>("vertexshader_depth");
	auto sh = ps->GetShader();
	vpMat = mainCam->GetViewProjectionMatrix();

	auto& dr = Engine::Get().GetGraphics().GetDeviceResources();
	float bc[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
	//dr.GetDeviceContext()->ClearDepthStencilView(dr.GetSubDepthStencilView(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	dr.GetDeviceContext()->ClearRenderTargetView(ShadowMap::GetInstance().m_ResultRenderTargetView.Get(), bc);
	dr.GetDeviceContext()->RSSetState(dr.GetRasterizerState());
	dr.GetDeviceContext()->OMSetDepthStencilState(dr.GetBaseDepthStencilState(), 0);
	dr.GetDeviceContext()->OMSetRenderTargets(1, ShadowMap::GetInstance().m_ResultRenderTargetView.GetAddressOf(), ShadowMap::GetInstance().m_DepthStencilView.Get());

	Core::Pool<RenderInfo>::GetInstance().ForEach(RenderModelResult);

	dr.GetDeviceContext()->PSSetShaderResources(1, 1, ShadowMap::GetInstance().ShadowCaster->GetShadowMapShaderResourceViewAddr());
}

void ShadowMap::OnGui()
{
	ImGui::Begin("ShadowMap - Again");
	ImGuiIO& io = ImGui::GetIO();
	ImVec2 scene_size = ImVec2(io.DisplaySize.x * 0.2f, io.DisplaySize.y * 0.2f);
	ImGui::Image(m_ShadowMapShaderResourceView.Get(), scene_size);
	ImGui::Image(m_ResultShaderResourceView.Get(), scene_size);
	ImGui::End();
}
