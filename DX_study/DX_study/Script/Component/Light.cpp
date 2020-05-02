#include "Light.h"
#include "Transform.h"
#include "../Internal/Core/GameObject.h"
#include "../Internal/Core/InternalHelper.h"
#include "../Internal/Engine/AdapterReader.h"
#include "../Internal/Graphics/imGui/imgui.h"
#include "../Util/ErrorLogger.h"
#include "../Internal/Graphics/Graphics.h"

using DirectX::operator+;

const float LightBase::LIGHT_STRENGTH_MAX = 100.0f;

void LightBase::Awake() {
	try {
		D3D11_TEXTURE2D_DESC textureDesc;
		ZeroMemory(&textureDesc, sizeof(textureDesc));
		textureDesc.Width = Core::GetGraphics().GetWindowWidth();
		textureDesc.Height = Core::GetGraphics().GetWindowHeight();
		textureDesc.MipLevels = 1;
		textureDesc.ArraySize = 1;
		textureDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
		textureDesc.SampleDesc.Count = 1;
		textureDesc.Usage = D3D11_USAGE_DEFAULT;
		textureDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
		textureDesc.CPUAccessFlags = 0;
		textureDesc.MiscFlags = 0;

		Microsoft::WRL::ComPtr<ID3D11Texture2D> texture2;
		ThrowIfFailed(
			Core::GetDevice()->CreateTexture2D(&textureDesc, NULL, m_ShadowMapRenderTargetTexture.GetAddressOf()),
			"Failed to create renderTargetTextureArr.");
		ThrowIfFailed(
			Core::GetDevice()->CreateTexture2D(&textureDesc, NULL, texture2.GetAddressOf()),
			"Failed to create renderTargetTextureArr.");

		D3D11_RENDER_TARGET_VIEW_DESC renderTargetViewDesc;
		renderTargetViewDesc.Format = textureDesc.Format;
		renderTargetViewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
		renderTargetViewDesc.Texture2D.MipSlice = 0;
		ThrowIfFailed(
			Core::GetDevice()->CreateRenderTargetView(m_ShadowMapRenderTargetTexture.Get(), &renderTargetViewDesc, m_ShadowMapRenderTargetView.GetAddressOf()),
			"Failed to create renderTargetViewArr.");
		ThrowIfFailed(
			Core::GetDevice()->CreateRenderTargetView(texture2.Get(), &renderTargetViewDesc, m_ResultRenderTargetView.GetAddressOf()),
			"Failed to create renderTargetViewArr.");

		D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc;
		shaderResourceViewDesc.Format = textureDesc.Format;
		shaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		shaderResourceViewDesc.Texture2D.MostDetailedMip = 0;
		shaderResourceViewDesc.Texture2D.MipLevels = 1;
		ThrowIfFailed(
			Core::GetDevice()->CreateShaderResourceView(m_ShadowMapRenderTargetTexture.Get(), &shaderResourceViewDesc, m_ShadowMapShaderResourceView.GetAddressOf()),
			"Failed to create shaderResourceViewArr.");
		ThrowIfFailed(
			Core::GetDevice()->CreateShaderResourceView(texture2.Get(), &shaderResourceViewDesc, m_ResultShaderResourceView.GetAddressOf()),
			"Failed to create shaderResourceViewArr.");
	}
	catch (CustomException & exception) {
		StringHelper::ErrorLog(exception);
	}
}

DirectX::XMMATRIX SpotLight::GetLightViewProjectMat() const
{
	auto vpMat = m_GameObject->GetTransform().GetViewMatrix() * m_ProjectionMatrix;
	return vpMat;
}

bool SpotLight::CullRenderable(const DirectX::BoundingBox & src)
{
	return m_Frustum.Contains(src) == DirectX::DISJOINT;
}

void SpotLight::ProcessLight()
{
}

void SpotLight::OnGui()
{
	LightBase::OnGui();
	float range = m_Range;
	float angle = m_SpotAngle;

	ImGui::DragFloat("Range", &range, 0.1f, 0.2f, 300.0f);
	ImGui::DragFloat3("Attenuation", &Attentuation.x, 0.01f, 0.0f, 10.0f);
	ImGui::DragFloat("SpotAngle", &angle, 0.1f, -180.0f, 180.0f);

	if (range != m_Range) SetRange(range);
	if (angle != m_SpotAngle) SetSpotAngle(angle);

	ImGuiIO& io = ImGui::GetIO();
	ImVec2 scene_size = ImVec2(io.DisplaySize.x * 0.2f, io.DisplaySize.y * 0.2f);
	ImGui::Image(m_ShadowMapShaderResourceView.Get(), scene_size);
}

void SpotLight::SetRange(float range)
{
	m_Range = range;
	SetProjectionMatrix();
}

void SpotLight::SetSpotAngle(float angle)
{
	m_SpotAngle = angle;
	SetProjectionMatrix();
}

void SpotLight::SetProjectionMatrix()
{
	float fovRadians = (m_SpotAngle / 360.0f) * DirectX::XM_2PI;
	m_ProjectionMatrix = DirectX::XMMatrixPerspectiveFovLH(fovRadians, 1280/720, 0.1f, m_Range);
}

void LightBase::OnGui()
{
	ImGui::ColorEdit3("Color", &Color.x, ImGuiColorEditFlags_NoAlpha);
	ImGui::DragFloat("Strength", &Strength, 0.1f, 0.0f, LIGHT_STRENGTH_MAX);
}

bool PointLight::CullRenderable(const DirectX::BoundingBox & target) {
	return Math::GetDistance(m_GameObject->GetTransform(), target.Center) > Range;
}