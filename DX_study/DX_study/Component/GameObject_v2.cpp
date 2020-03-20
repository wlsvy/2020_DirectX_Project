#include "GameObject_v2.h"

GameObject_v2::GameObject_v2(SceneManager * const sceneM, Model * model, const int & vshaderID, const int & pshaderID, const DirectX::XMFLOAT3 & pos, const DirectX::XMFLOAT3 & rot)
	: sceneManager(sceneM),
	transform(COMPONENT_INIT_DESC(this, sceneM->Time, nullptr, nullptr)),
	renderer(COMPONENT_INIT_DESC(this, sceneM->Time, &transform, nullptr))
{
	//renderer.Renderer_Initialize(model, vshaderID, pshaderID);

	this->transform.SetPosition(pos);
	this->transform.SetRotation(rot);
}

GameObject_v2::GameObject_v2(const GAMEOBJECT_INIT_DESC & desc) :
	sceneManager(desc.scene_manager),
	mGameObjectID(desc.obj_id),
	transform(COMPONENT_INIT_DESC(this, desc.scene_manager->Time, nullptr, nullptr)),
	renderer(COMPONENT_INIT_DESC(this, desc.scene_manager->Time, &transform, nullptr))
{
	renderer.Initialize(desc.model, 
		desc.vshaderPtr, 
		desc.pshaderPtr, 
		desc.gshaderPtr, 
		sceneManager->getDevicePtr(),
		sceneManager->getDeviceContextPtr());

	std::strcpy(mGameObjectName, desc.obj_name.c_str());

	this->transform.SetPosition(desc.pos);
	this->transform.SetRotation(desc.rot);
	this->transform.SetScale(desc.scale);
}

GameObject_v2::~GameObject_v2()
{
	for (std::vector<std::shared_ptr<Component>>::iterator it = componentBuffer.begin(); it != componentBuffer.end(); it++) {
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

	for (auto ptr : componentBuffer)
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
	sceneManager->Destory_GameObject(this);
}

void GameObject_v2::Destroy(GameObject_v2 * _target)
{
	_target->Destroy();
}
