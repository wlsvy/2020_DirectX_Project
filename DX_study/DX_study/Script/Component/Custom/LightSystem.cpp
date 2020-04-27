#include "LightSystem.h"

#include "../Light.h"
#include "../../Internal/Graphics/imGui/imgui.h"
#include "../../Internal/Graphics/Graphics.h"
#include "../../Internal/Core/InternalHelper.h"

void LightSystem::Update() {
	Core::GetGraphics().GetCbAmbientLight().ApplyChanges();
}

void LightSystem::OnGui()
{
	auto & cbAmbientLight = Core::GetGraphics().GetCbAmbientLight();
	ImGui::DragFloat("Ambient Strength", &cbAmbientLight.data.ambientLightStrength, 0.01f, 0.0f, 10.0f);
	ImGui::ColorEdit3("Ambient Color", &cbAmbientLight.data.ambientLightColor.x, ImGuiColorEditFlags_NoAlpha);
}
