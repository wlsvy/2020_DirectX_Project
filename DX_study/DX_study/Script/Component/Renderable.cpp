#include "Renderable.h"
#include "../Internal/Graphics/imGui/imgui.h"
#include "../Internal/Graphics/Mesh.h"
#include "../Internal/Graphics/Model.h"
#include "../Util/Math.h"
#include "../Util/StringHelper.h"

void Renderable::OnGui() {
	if (auto& model = Model) {
		for (auto& mesh : model->GetMeshes()) {
			auto& c = mesh.GetLocalAABB().Center;
			auto& e = mesh.GetLocalAABB().Extents;
			ImGui::Text((mesh.Name + " - AABB : \n" + std::to_string(c) + "\n" + std::to_string(e) + "\n").c_str());
		}
	}
	if (auto& model = SkinnedModel) {
		for (auto& mesh : model->GetMeshes()) {
			auto& c = mesh.GetLocalAABB().Center;
			auto& e = mesh.GetLocalAABB().Extents;
			ImGui::Text((mesh.Name + " - AABB : \n" + std::to_string(c) + "\n" + std::to_string(e) + "\n").c_str());
		}
	}

	ImGui::Checkbox("Is Visible", &m_IsVisible);
}