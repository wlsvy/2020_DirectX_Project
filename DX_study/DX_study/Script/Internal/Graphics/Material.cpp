#include "Material.h"
#include "../Core/ObjectPool.h"
#include "../Engine/Ui.h"
#include "Shaders.h"
#include "Texture.h"

std::shared_ptr<SharedMaterial> SharedMaterial::GetDefault() {
	static std::weak_ptr<SharedMaterial> s_Default = Core::Find<SharedMaterial>("Default");
	return s_Default.lock();
}

#define TEXTURE_SELECTION_POPUP(target, guiKey)									\
ImGui::Text(#target);															\
ImGui::SameLine();																\
if (target) {																	\
	ImGui::Image(target->GetTextureResourceView(), imageSize);					\
	ImGui::SameLine();															\
	if (ImGui::Button(target->Name.c_str(), guiKey)) {							\
		ImGui::OpenPopup(guiKey);												\
	}																			\
}																				\
else {																			\
	if (ImGui::Button("No Texture", guiKey)) {									\
		ImGui::OpenPopup(guiKey);												\
	}																			\
}																				\
																				\
if (ImGui::BeginPopup(guiKey))													\
{																				\
	ImGui::Text("Texture");														\
	ImGui::Separator();															\
	for (auto & texture : Core::Pool<Texture>::GetInstance().GetItems()) {		\
		ImGui::Image(texture->GetTextureResourceView(), imageSize);				\
		ImGui::SameLine();														\
		if (ImGui::Selectable(texture->Name.c_str())) {							\
			target = texture;													\
		}																		\
	}																			\
	ImGui::EndPopup();															\
}																				\

void Material::OnGui(const char* option)
{
	auto displaySize = GUI::GetDisplaySize();
	auto imageSize = ImVec2(displaySize.x * 0.02f, displaySize.y * 0.02f);

	ImGui::PushItemWidth(displaySize.x * 0.15f);

	{
		char albedoTextureGuiKey[15];
		std::sprintf(albedoTextureGuiKey, "%s_M%d_A", option, GetId());

		TEXTURE_SELECTION_POPUP(Albedo, albedoTextureGuiKey);
		ImGui::ColorEdit4(albedoTextureGuiKey, "Albedo Color", &Color.x);
	}
	{
		char normalTextureGuiKey[15];
		std::sprintf(normalTextureGuiKey, "%s_M%d_N", option, GetId());

		TEXTURE_SELECTION_POPUP(Normal, normalTextureGuiKey);
		ImGui::SliderFloat("Normal Intensity", &NormalIntensity, 0.0f, 1.0f);
	}
	{
		char metalTextureGuiKey[15];
		std::sprintf(metalTextureGuiKey, "%s_M%d_M", option, GetId());

		TEXTURE_SELECTION_POPUP(Metal, metalTextureGuiKey);
		ImGui::SliderFloat("Metal Intensity", &MetalIntensity, 0.0f, 1.0f);
	}
	{
		char roughTextureGuiKey[15];
		std::sprintf(roughTextureGuiKey, "%s_M%d_R", option, GetId());

		TEXTURE_SELECTION_POPUP(Roughness, roughTextureGuiKey);
		ImGui::SliderFloat("Roughness Intensity", &RoughnessIntensity, 0.0f, 1.0f);
	}
	{
		char emissionTextureGuiKey[15];
		std::sprintf(emissionTextureGuiKey, "%s_M%d_E", option, GetId());

		TEXTURE_SELECTION_POPUP(Emission, emissionTextureGuiKey);
		ImGui::ColorEdit4(emissionTextureGuiKey, "Emission Color", &EmissionColor.x);
		ImGui::SliderFloat("Emission Intensity", &EmissionIntensity, 0.0f, 1.0f);
	}

	ImGui::PopItemWidth();
}

std::shared_ptr<ShaderState> ShaderState::GetDefault() {
	static std::weak_ptr<ShaderState> s_Default = Core::Find<ShaderState>("Mesh");
	return s_Default.lock();
}

std::shared_ptr<ShaderState> ShaderState::GetSkinnedDefault()
{
	static std::weak_ptr<ShaderState> s_Default = Core::Find<ShaderState>("SkinnedMesh");
	return s_Default.lock();
}

void ShaderState::OnGui(const char* option) {
	if (Vshader) {
		ImGui::Text(("Vertex : " + Vshader->Name).c_str());
	}
	if (Gshader) {
		ImGui::Text(("Geometry : " + Gshader->Name).c_str());
	}
	if (Pshader) {
		ImGui::Text(("Pixel : " + Pshader->Name).c_str());
	}
}