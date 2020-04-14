#include "GameObject.h"

#include <algorithm>
#include "ObjectPool.h"
#include "InternalHelper.h"
#include "../../Component/Transform.h"
#include "../../Component/Renderable.h"

#include "../Graphics/imGui/imgui.h"

GameObject::GameObject() : 
	m_Transform(std::make_shared<Transform>(this)),
	m_Renderer(Pool::CreateInstance<Renderable>(this)),
	Object("GameObject")
{
	m_Transform->SetParent(Core::GetWorldTransform());
}

GameObject::GameObject(const std::string & name) :
	m_Transform(std::make_shared<Transform>(this)),
	m_Renderer(Pool::CreateInstance<Renderable>(this)),
	Object(name)
{
	m_Transform->SetParent(Core::GetWorldTransform());
}

GameObject::~GameObject()
{
}

void GameObject::RemoveExpiredComponent()
{
	if (m_Components.size() == 0) return;
	std::remove_if(m_Components.begin(), m_Components.end(), 
		[](std::weak_ptr<Component>& ptr)
	{
		return ptr.expired();
	});
}

void GameObject::OnGui()
{
	ImGuiInputTextFlags flags = ImGuiInputTextFlags_AutoSelectAll;
	ImGui::InputText("Name", &Name[0], flags);
	ImGui::SameLine();
	ImGui::Text("(ID : %d)", GetId());

	ImGui::Spacing();
	ImGui::BeginChild(GetId(), ImVec2(0, 0), false); // ID는 0 이 되면 안됨

	ImGuiTreeNodeFlags node_flags = ImGuiTreeNodeFlags_CollapsingHeader
		| ImGuiTreeNodeFlags_OpenOnArrow
		| ImGuiTreeNodeFlags_OpenOnDoubleClick
		| ImGuiTreeNodeFlags_DefaultOpen;

	if (ImGui::CollapsingHeader(m_Transform->Name.c_str(), node_flags))
	{
		ImGui::Spacing();
		m_Transform->OnGui();
	}
	ImGui::Spacing();

	for (auto& wp : m_Components)
	{
		if (auto ptr = wp.lock())
		{
			//ImGui::Checkbox("", &ptr->enabled);
			ImGui::SameLine();
			if (ImGui::CollapsingHeader(ptr->Name.c_str(), node_flags))
			{
				ImGui::Spacing();
				ptr.get()->OnGui();
			}
			ImGui::Spacing();
		}
	}

	/*if (ImGui::CollapsingHeader(renderer.mComponentName, node_flags))
	{
		ImGui::Spacing();
		renderer.OnGui();
	}
	ImGui::Spacing();*/

	ImGui::Separator();
	ImGui::Spacing();
	ImGui::EndChild();
}

