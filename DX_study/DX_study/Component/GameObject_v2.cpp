#include "GameObject_v2.h"
#include "../Engine/ModuleResource.h"
#include "../Engine/Engine.h"

GameObject_v2::GameObject_v2(SceneManager * const sceneM, Model * model, const int & vshaderID, const int & pshaderID, const DirectX::XMFLOAT3 & pos, const DirectX::XMFLOAT3 & rot)
	: sceneManager(sceneM),
	transform(COMPONENT_INIT_DESC(this, nullptr, nullptr)),
	renderer(COMPONENT_INIT_DESC(this, &transform, nullptr))
{
	//renderer.Renderer_Initialize(model, vshaderID, pshaderID);

	this->transform.SetPosition(pos);
	this->transform.SetRotation(rot);
}

GameObject_v2::GameObject_v2(const GAMEOBJECT_INIT_DESC & desc) :
	sceneManager(desc.scene_manager),
	mGameObjectID(desc.obj_id),
	transform(COMPONENT_INIT_DESC(this, nullptr, nullptr)),
	renderer(COMPONENT_INIT_DESC(this, &transform, nullptr))
{
	renderer.Initialize(
		desc.model, 
		desc.vshaderPtr, 
		desc.pshaderPtr, 
		desc.gshaderPtr);

	std::strcpy(mGameObjectName, desc.obj_name.c_str());

	this->transform.SetPosition(desc.pos);
	this->transform.SetRotation(desc.rot);
	this->transform.SetScale(desc.scale);
}

GameObject_v2::~GameObject_v2()
{
	for (auto it = m_Components.begin(); it != m_Components.end(); it++) {
		//delete it;
	}
}

int GameObject_v2::getID()
{
	return mGameObjectID;
}

void GameObject_v2::OnGui()
{
	ImGuiInputTextFlags flags = ImGuiInputTextFlags_AutoSelectAll;
	ImGui::InputText("Name", mGameObjectName, flags);
	ImGui::SameLine();
	ImGui::Text("(%d)", mGameObjectID);

	if (ImGui::Checkbox("Enable", &enabled))
	{
		//set_active(active_);
	}
	

	
	//ImGui::SameLine();
	

	ImGui::Spacing();
	ImGui::BeginChild(mGameObjectID, ImVec2(0,0), false); // ID는 0 이 되면 안됨

	ImGuiTreeNodeFlags node_flags = ImGuiTreeNodeFlags_CollapsingHeader
		| ImGuiTreeNodeFlags_OpenOnArrow
		| ImGuiTreeNodeFlags_OpenOnDoubleClick
		| ImGuiTreeNodeFlags_DefaultOpen;

	if (ImGui::CollapsingHeader(transform.mComponentName, node_flags))
	{
		ImGui::Spacing();
		transform.OnGui();
	}
	ImGui::Spacing();

	for (auto ptr : m_Components)
	{
		if (ptr.get() != nullptr)
		{
			ImGui::Checkbox("", &ptr->enabled);
			ImGui::SameLine();
			if (ImGui::CollapsingHeader(ptr.get()->mComponentName, node_flags))
			{
				ImGui::Spacing();
				ptr.get()->OnGui();
			}
			ImGui::Spacing();
		}
	}

	if (ImGui::CollapsingHeader(renderer.mComponentName, node_flags))
	{
		ImGui::Spacing();
		renderer.OnGui();
	}
	ImGui::Spacing();

	ImGui::Separator();
	ImGui::Spacing();
	ImGui::EndChild();
}

void GameObject_v2::Destroy()
{
	sceneManager->DestoryGameObject(this);
}

void GameObject_v2::Destroy(GameObject_v2 * _target)
{
	_target->Destroy();
}

void GameObject_v2::RegisterComponent(std::shared_ptr<ScriptBehaviour> compo, ScriptComponentTag)
{
	Module::GetEngine().RegisterComponent(compo);
}

void GameObject_v2::RegisterComponent(std::shared_ptr<Light_ver2> compo, LigthComponentTag)
{
	Module::GetEngine().RegisterComponent(compo);
}

void GameObject_v2::RegisterComponent(std::shared_ptr<Collider_v2> compo, PhysicsComponentTag)
{
	Module::GetEngine().RegisterComponent(compo);
}

void GameObject_v2::RegisterComponent(std::shared_ptr<Animator> compo, AnimationComponentTag)
{
	Module::GetEngine().RegisterComponent(compo);
}

void GameObject_v2::RegisterComponent(std::shared_ptr<Terrain> compo, TerrainComponentTag)
{
	Module::GetEngine().RegisterComponent(compo);
}

//void GameObject_v2::DeRegisterComponent(std::shared_ptr<ScriptBehaviour> compo, ScriptComponentTag)
//{
//	Module::DeregisterComponent(compo);
//}
//
//void GameObject_v2::DeRegisterComponent(std::shared_ptr<Light_ver2> compo, LigthComponentTag)
//{
//	Module::DeregisterComponent(compo);
//}
//
//void GameObject_v2::DeRegisterComponent(std::shared_ptr<Collider_v2> compo, PhysicsComponentTag)
//{
//	Module::DeregisterComponent(compo);
//}
//
//void GameObject_v2::DeRegisterComponent(std::shared_ptr<Animator> compo, AnimationComponentTag)
//{
//	Module::DeregisterComponent(compo);
//}
//
//void GameObject_v2::DeRegisterComponent(std::shared_ptr<Terrain> compo, TerrainComponentTag)
//{
//	Module::DeregisterComponent(compo);
//}