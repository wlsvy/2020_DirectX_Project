#include "LightSystem.h"

#include "../Light.h"
#include "../../Internal/Engine/Ui.h"
#include "../../Internal/Graphics/Graphics.h"
#include "../../Internal/Core/InternalHelper.h"

void LightSystem::OnGui()
{
	auto & cbScene = Core::GetGraphics().GetCbScene();
	auto & cbFurData = Core::GetGraphics().GetCbFurData();
	ImGui::DragFloat("Ambient Strength", &cbScene.data.ambientLightStrength, 0.01f, 0.0f, 10.0f);
	ImGui::ColorEdit3("Ambient Color", &cbScene.data.ambientLightColor.x, ImGuiColorEditFlags_NoAlpha);
	ImGui::DragFloat("SSAO Strength", &cbScene.data.ssao_Strength, 0.01f, 0.0f, 10.0f);
	ImGui::DragFloat("SSAO Radius", &cbScene.data.ssao_Radius, 0.01f, 0.0f, 10.0f);
	ImGui::DragFloat("SSAO Bias", &cbScene.data.ssao_Bias, 0.01f, 0.0f, 10.0f);
	ImGui::DragFloat("SSAO Scale", &cbScene.data.ssao_Scale, 0.01f, 0.0f, 10.0f);
	ImGui::DragFloat("softShadowInterpoloateBias", &cbScene.data.softShadowInterpoloateBias, 0.01f, 0.0f, 10.0f);
	ImGui::DragFloat("softShadowPCFBias", &cbScene.data.softShadowPCFBias, 0.01f, 0.0f, 10.0f);
	ImGui::DragFloat("softShadowPCFIter", &cbScene.data.softShadowPCFIter, 1.0f, 0.0f, 10.0f);

	ImGui::DragFloat3("VolumetricLight", &cbScene.data.VolumetricLightVar.x, 0.01f, 0.0f, 10.0f);
	ImGui::DragInt("VolumetricLightSampleCount", &cbScene.data.VolumetricLightSampleCount, 1, 0, 100);

	ImGui::DragInt("FurLayer", &cbFurData.data.FurLayer, 1, 0, 100);
	ImGui::DragFloat("FurLength", &cbFurData.data.FurLength, 0.01f, 0.0f, 100.0f);
	ImGui::DragFloat("FurOpacityThreshold", &cbFurData.data.FurOpacityThreshold, 0.01f, 0.0f, 1.0f);
	ImGui::DragFloat("FurOpacity", &cbFurData.data.FurOpacity, 0.01f, 0.0f, 10000.0f);
	ImGui::DragFloat("FurDensity", &cbFurData.data.FurDensity, 0.01f, 0.0f, 10000.0f);
	ImGui::DragFloat("FurGravity", &cbFurData.data.FurGravity, 0.01f, 0.0f, 10000.0f);
}
