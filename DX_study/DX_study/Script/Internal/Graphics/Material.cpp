#include "Material.h"
#include "../Core/ObjectPool.h"
#include "../Engine/Ui.h"
#include "Shaders.h"
#include "Texture.h"

std::shared_ptr<SharedMaterial> SharedMaterial::GetDefault() {
	static std::weak_ptr<SharedMaterial> s_Default = Core::Find<SharedMaterial>("Default");
	return s_Default.lock();
}

void Material::OnGui()
{
	char guiTexturePopUpKey[10];
	char guiShaderNodeKey[10];
	std::sprintf(guiTexturePopUpKey, "TpopUp_%d", GetId());
	std::sprintf(guiShaderNodeKey, "Snode_%d", GetId());

	//ImGui::ColorEdit4(guiTexturePopUpKey, "Albedo Color", &Color.x);

	ImGui::Text("Albedo : ");
	ImGui::SameLine();
	if (Albedo) {
		ImGui::SameLine();
		if (ImGui::Button(Albedo->Name.c_str())) {
			ImGui::OpenPopup(guiTexturePopUpKey);
		}
	}
	else {
		if (ImGui::Button("No Albedo Texture")) {
			ImGui::OpenPopup(guiTexturePopUpKey);
		}
	}

	ImGui::Text("Normal : ");
	ImGui::SameLine();
	if (Normal) {
		ImGui::SameLine();
		if (ImGui::Button(Normal->Name.c_str())) {
			ImGui::OpenPopup(guiTexturePopUpKey);
		}
	}
	else {
		if (ImGui::Button("No Normal Texture")) {
			ImGui::OpenPopup(guiTexturePopUpKey);
		}
	}

	if (ImGui::BeginPopup(guiTexturePopUpKey))
	{
		ImGui::Text("Texture");
		ImGui::Separator();
		for (auto & texture : Core::Pool<Texture>::GetInstance().GetItems()) {
			if (ImGui::Selectable(texture->Name.c_str())) {
				Normal = texture;
			}
		}
		ImGui::EndPopup();
	}



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