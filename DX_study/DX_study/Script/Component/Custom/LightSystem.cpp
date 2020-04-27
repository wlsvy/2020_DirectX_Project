#include "LightSystem.h"

#include "../Light.h"
#include "../../Internal/Graphics/imGui/imgui.h"
#include "../../Internal/Graphics/Graphics.h"
#include "../../Internal/Core/InternalHelper.h"

void LightSystem::Update() {
	//Core::GetGraphics().GetCbAmbientLight().ApplyChanges();
}

void LightSystem::OnGui()
{
	auto & cbScene = Core::GetGraphics().GetCbScene();
	ImGui::DragFloat("Ambient Strength", &cbScene.data.ambientLightStrength, 0.01f, 0.0f, 10.0f);
	ImGui::ColorEdit3("Ambient Color", &cbScene.data.ambientLightColor.x, ImGuiColorEditFlags_NoAlpha);
}
