#include "Light.h"
#include "Transform.h"
#include "../Internal/Core/GameObject.h"
#include "../Internal/Core/InternalHelper.h"
#include "../Internal/Engine/AdapterReader.h"
#include "../Internal/Graphics/imGui/imgui.h"
#include "../Util/ErrorLogger.h"

using DirectX::operator+;

const float LightBase::LIGHT_STRENGTH_MAX = 100.0f;

void LightBase::Awake() {
	try {
		D3D11_TEXTURE2D_DESC textureDesc;
		ZeroMemory(&textureDesc, sizeof(textureDesc));
		textureDesc.Width = 1280;
		textureDesc.Height = 720;
		textureDesc.MipLevels = 1;
		textureDesc.ArraySize = 1;
		textureDesc.Format = DXGI_FORMAT_R8_UNORM;
		textureDesc.SampleDesc.Count = 1;
		textureDesc.Usage = D3D11_USAGE_DEFAULT;
		textureDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
		textureDesc.CPUAccessFlags = 0;
		textureDesc.MiscFlags = 0;
		ThrowIfFailed(
			Core::GetDevice()->CreateTexture2D(&textureDesc, NULL, m_ShadowMapRenderTargetTexture.GetAddressOf()),
			"Failed to create renderTargetTextureArr.");

		D3D11_RENDER_TARGET_VIEW_DESC renderTargetViewDesc;
		renderTargetViewDesc.Format = textureDesc.Format;
		renderTargetViewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
		renderTargetViewDesc.Texture2D.MipSlice = 0;
		ThrowIfFailed(
			Core::GetDevice()->CreateRenderTargetView(m_ShadowMapRenderTargetTexture.Get(), &renderTargetViewDesc, m_ShadowMapRenderTargetView.GetAddressOf()),
			"Failed to create renderTargetViewArr.");

		D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc;
		shaderResourceViewDesc.Format = textureDesc.Format;
		shaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		shaderResourceViewDesc.Texture2D.MostDetailedMip = 0;
		shaderResourceViewDesc.Texture2D.MipLevels = 1;
		ThrowIfFailed(
			Core::GetDevice()->CreateShaderResourceView(m_ShadowMapRenderTargetTexture.Get(), &shaderResourceViewDesc, m_ShadowMapShaderResourceView.GetAddressOf()),
			"Failed to create shaderResourceViewArr.");
	}
	catch (CustomException & exception) {
		ErrorLogger::Log(exception);
	}
}

DirectX::XMMATRIX SpotLight::GetLightViewProjectMat() const
{
	return m_GameObject->GetTransform().GetViewMatrix() * m_ProjectionMatrix;
}

bool SpotLight::CullRenderable(const DirectX::BoundingBox & src)
{
	return m_Frustum.Contains(src) != DirectX::DISJOINT;
}

void SpotLight::ProcessLight()
{
}

void SpotLight::OnGui()
{
	LightBase::OnGui();
	float range = m_Range;
	float angle = m_SpotAngle;

	ImGui::DragFloat("Range", &range, 0.1f, 0.0f, 300.0f);
	ImGui::DragFloat("SpotAngle", &angle, 0.1f, -180.0f, 180.0f);

	if (range != m_Range) SetRange(range);
	if (angle != m_SpotAngle) SetSpotAngle(angle);
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
	m_ProjectionMatrix = DirectX::XMMatrixPerspectiveFovLH(fovRadians, 1.0f, 0.1f, m_Range);
}

void LightBase::OnGui()
{
	ImGui::ColorEdit3("Color", &Color.x, ImGuiColorEditFlags_NoAlpha);
	ImGui::DragFloat("Strength", &Strength, 0.1f, 0.0f, LIGHT_STRENGTH_MAX);
}

bool PointLight::CullRenderable(const DirectX::BoundingBox & target) {
	return Math::GetDistance(m_GameObject->GetTransform(), target.Center) < Range;
}