#include "RenderInfo.h"
#include "../Internal/Engine/Ui.h"
#include "../Internal/Graphics/Mesh.h"
#include "../Internal/Graphics/Model.h"
#include "../Internal/Core/ObjectPool.h"
#include "../Util/Math.h"
#include "../Util/StringHelper.h"

void RenderInfo::SetModel(const std::shared_ptr<Model>& model) {
	if(!model){
		return;
	}

	m_Model = model;
	m_Renderables = model->GetDefaultRenderables();
}

void RenderInfo::OnGui(const char* option) {
	
	ImGui::Text("Model : ");
	ImGui::SameLine();
	if (!m_Model &&
		ImGui::Button("No Model Selected")) 
	{
		ImGui::OpenPopup("RenderInfo_SetModel_PopUp");
	}
	if (m_Model && 
		ImGui::Button(m_Model->Name.c_str())) 
	{
		ImGui::OpenPopup("RenderInfo_SetModel_PopUp");
	}

	if (ImGui::BeginPopup("RenderInfo_SetModel_PopUp"))
	{
		ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "Model");
		ImGui::Separator();
		for (auto & model : Core::Pool<Model>::GetInstance().GetItems()) {
			if (ImGui::Selectable(model->Name.c_str())) {
				SetModel(model);
			}
		}
		ImGui::EndPopup();
	}

	ImGui::Separator();

	if (m_Model) {
		char index[2] = { '0', '\0' };
		for (auto & r : m_Renderables) {
			r.GetMesh()->OnGui();

			auto mat = r.GetMaterial();
			char guiMatKey[15], guiMatProperty[15];
			std::sprintf(guiMatKey, "GuiM%s_%d", index, mat->GetId());
			std::sprintf(guiMatProperty, "GuiMP%s_%d", index, mat->GetId());

			ImGui::Text("Material : ");
			ImGui::SameLine();
			if (ImGui::Button(mat->Name.c_str(), guiMatKey))
			{
				ImGui::OpenPopup(guiMatKey);
			}

			if (ImGui::BeginPopup(guiMatKey))
			{
				ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "Material");
				ImGui::Separator();
				for (auto & mat : Core::Pool<SharedMaterial>::GetInstance().GetItems()) {
					if (ImGui::Selectable(mat->Name.c_str())) {
						r.SetMaterial(mat);
					}
				}
				ImGui::EndPopup();
			}

			if (ImGui::TreeNode(guiMatProperty, "Property")) {
				mat->OnGui(index);
				ImGui::TreePop();
			}

			auto shaderState = r.GetShaderState();
			char guiShaderKey[15], guiShaderProperty[15];
			std::sprintf(guiShaderKey, "GuiS%s_%d", index, shaderState->GetId());
			std::sprintf(guiShaderProperty, "GuiSP%s_%d", index, shaderState->GetId());

			ImGui::Text("ShaderState : ");
			ImGui::SameLine();
			if (ImGui::Button(shaderState->Name.c_str(), guiShaderKey))
			{
				ImGui::OpenPopup(guiShaderKey);
			}

			if (ImGui::BeginPopup(guiShaderKey))
			{
				ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "ShaderState");
				ImGui::Separator();
				for (auto & shaderState : Core::Pool<ShaderState>::GetInstance().GetItems()) {
					if (ImGui::Selectable(shaderState->Name.c_str())) {
						r.SetShaderState(shaderState);
					}
				}
				ImGui::EndPopup();
			}
			if (ImGui::TreeNode(guiShaderProperty, "Property")) {
				shaderState->OnGui(index);
				ImGui::TreePop();
			}
			ImGui::Separator();

			index[0]++;
		}
	}

	ImGui::Checkbox("Is Visible", &m_IsVisible);
}

