#include "LightSystem.h"

#include "../Light.h"
#include "../../Internal/Graphics/imGui/imgui.h"
#include "../../Internal/Graphics/Graphics.h"
#include "../../Internal/Core/InternalHelper.h"

void LightSystem::OnGui()
{
	auto & cbScene = Core::GetGraphics().GetCbScene();
	ImGui::DragFloat("Ambient Strength", &cbScene.data.ambientLightStrength, 0.01f, 0.0f, 10.0f);
	ImGui::ColorEdit3("Ambient Color", &cbScene.data.ambientLightColor.x, ImGuiColorEditFlags_NoAlpha);
	ImGui::DragFloat("SSAO Strength", &cbScene.data.ssao_Strength, 0.01f, 0.0f, 10.0f);
	ImGui::DragFloat("SSAO Radius", &cbScene.data.ssao_Radius, 0.01f, 0.0f, 10.0f);
	ImGui::DragFloat("SSAO Bias", &cbScene.data.ssao_Bias, 0.01f, 0.0f, 10.0f);
	ImGui::DragFloat("SSAO Scale", &cbScene.data.ssao_Scale, 0.01f, 0.0f, 10.0f);
	ImGui::DragFloat3("VolumetricLight", &cbScene.data.VolumetricLightVar.x, 0.01f, 0.0f, 10.0f);
	ImGui::DragInt("VolumetricLightSampleCount", &cbScene.data.VolumetricLightSampleCount, 1, 0, 100);
}
