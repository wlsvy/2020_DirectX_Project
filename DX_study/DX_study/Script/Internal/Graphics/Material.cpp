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
	if (ImGui::Button(target->Name.c_str())) {									\
		ImGui::OpenPopup(guiKey);												\
	}																			\
}																				\
else {																			\
	if (ImGui::Button("No Texture")) {											\
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

void Material::OnGui()
{
	auto displaySize = GUI::GetDisplaySize();
	auto imageSize = ImVec2(displaySize.x * 0.02f, displaySize.y * 0.02f);

	ImGui::PushItemWidth(displaySize.x * 0.15f);

	{
		char albedoTextureGuiKey[10];
		std::sprintf(albedoTextureGuiKey, "guiA_%d", GetId());

		TEXTURE_SELECTION_POPUP(Albedo, albedoTextureGuiKey);
		ImGui::ColorEdit4(albedoTextureGuiKey, "Albedo Color", &Color.x);
	}
	{
		char normalTextureGuiKey[10];
		std::sprintf(normalTextureGuiKey, "guiN_%d", GetId());

		TEXTURE_SELECTION_POPUP(Normal, normalTextureGuiKey);
		ImGui::SliderFloat("Normal Intensity", &NormalIntensity, 0.0f, 1.0f);
	}
	{
		char metalTextureGuiKey[10];
		std::sprintf(metalTextureGuiKey, "guiM_%d", GetId());

		TEXTURE_SELECTION_POPUP(Metal, metalTextureGuiKey);
		ImGui::SliderFloat("Metal Intensity", &MetalIntensity, 0.0f, 1.0f);
	}
	{
		char roughTextureGuiKey[10];
		std::sprintf(roughTextureGuiKey, "guiR_%d", GetId());

		TEXTURE_SELECTION_POPUP(Roughness, roughTextureGuiKey);
		ImGui::SliderFloat("Roughness Intensity", &RoughnessIntensity, 0.0f, 1.0f);
	}
	{
		char specularTextureGuiKey[10];
		std::sprintf(specularTextureGuiKey, "guiS_%d", GetId());

		TEXTURE_SELECTION_POPUP(Specular, specularTextureGuiKey);
		ImGui::SliderFloat("Specular Intensity", &SpecularIntensity, 0.0f, 1.0f);
	}

	ImGui::PopItemWidth();
	
	char guiShaderNodeKey[10];
	std::sprintf(guiShaderNodeKey, "Snode_%d", GetId());

	if (ImGui::TreeNode(guiShaderNodeKey, "Shader")) {
		if (Vshader) {
			ImGui::Text(("Vertex : " + Vshader->Name).c_str());
		}
		if (Gshader) {
			ImGui::Text(("Geometry : " + Gshader->Name).c_str());
		}
		if (Pshader) {
			ImGui::Text(("Pixel : " + Pshader->Name).c_str());
		}
		ImGui::TreePop();
	}

}