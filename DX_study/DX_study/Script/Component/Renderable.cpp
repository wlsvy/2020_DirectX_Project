#include "Renderable.h"
#include "../Internal/Engine/Ui.h"
#include "../Internal/Graphics/Mesh.h"
#include "../Internal/Graphics/Model.h"
#include "../Internal/Core/ObjectPool.h"
#include "../Util/Math.h"
#include "../Util/StringHelper.h"

void RenderInfo::SetModel(const std::shared_ptr<Model>& model) {
	m_Model = model;
	m_Renderables.clear();

	auto & meshes = model->GetMeshes();
	auto & mats = model->GetDefaultMaterials();

	for (int i = 0; i < meshes.size(); i++) {
		m_Renderables.emplace_back(meshes[i], mats[i]);
	}
}

void RenderInfo::OnGui() {
	
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
		for (auto & r : m_Renderables) {
			r.GetMesh()->OnGui();

			auto mat = r.GetMaterial();
			char guiMatKey[15], guiMatPropertyKey[15];
			std::sprintf(guiMatKey, "MatPopUp_%d", mat->GetId());
			std::sprintf(guiMatPropertyKey, "MatProp_%d", mat->GetId());

			ImGui::Text("Material : ");
			ImGui::SameLine();
			if (ImGui::Button(mat->Name.c_str()))
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

			if (ImGui::TreeNode(guiMatPropertyKey, "Property")) {
				mat->OnGui();
				ImGui::TreePop();
			}
			ImGui::Separator();
		}
	}

	ImGui::Checkbox("Is Visible", &m_IsVisible);
}

