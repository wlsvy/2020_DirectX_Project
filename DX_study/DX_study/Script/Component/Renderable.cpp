#include "Renderable.h"
#include "../Internal/Graphics/imGui/imgui.h"
#include "../Internal/Graphics/Mesh.h"
#include "../Internal/Graphics/Model.h"
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
	
	if (auto& model = m_Model) {
		ImGui::Text(("Model : " + model->Name).c_str());
		for (auto & r : m_Renderables) {
			r.GetMesh()->OnGui();
			r.GetMaterial()->OnGui();
		}
		for (auto& mesh : model->GetMeshes()) {
			auto& c = mesh->GetLocalAABB().Center;
			auto& e = mesh->GetLocalAABB().Extents;
			ImGui::Text((mesh->Name + " - AABB : \n" + std::to_string(c) + "\n" + std::to_string(e) + "\n").c_str());
		}
	}

	ImGui::Checkbox("Is Visible", &m_IsVisible);
}

