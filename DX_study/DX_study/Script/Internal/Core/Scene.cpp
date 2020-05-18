#include "Scene.h"
#include "../../Component/Transform.h"

#include "../Engine/Ui.h"
#include "../Graphics/Graphics.h"
#include "../Graphics/Model.h"
#include "../Graphics/Shaders.h"
#include "../Graphics/AnimationClip.h"
#include "../Graphics/Material.h"
#include "../Graphics/Texture.h"

#include "GameObject.h"
#include "ImportHelper.h"
#include "../Engine/Engine.h"
#include "../../Component/Transform.h"
#include "../../Component/RenderInfo.h"
#include "../../Component/Animator.h"
#include "../../Component/Light.h"
#include "../../Component/Custom/CamMove.h"
#include "../../Component/Custom/CharacterMove.h"
#include "../../Component/Custom/LightSystem.h"
#include "../../GameObject/Camera.h"

bool Scene::s_IsInitialized = false;
const float Scene::SCENE_MAX_SIZE = 9999.0f;
const float Scene::SCENE_MIN_SIZE = -9999.0f;

Scene::Scene() : m_WorldTransform(std::make_shared<Transform>(nullptr, "World Transform"))
{
}

void Scene::Initialize()
{
	{
		auto manager = Core::CreateInstance<GameObject>("Prototype Manager");
		//auto ls = Core::CreateInstance<GameObject>("Light System");
		manager->AddComponent<LightSystem>();
		//ls->GetTransform().SetParent(manager->GetTransformPtr());
	}

	ProcessGameObjectTable();
	Core::Find<GameObject>("X_Bot")->GetRendererable().Anim->SetClip(Core::Find<AnimationClip>("X_Bot_Idle"));
	Core::Find<GameObject>("X_Bot")->GetRendererable().Anim->Play();

	{
		auto sphere = Core::CreateInstance<GameObject>("Cube");
		sphere->GetTransform().SetPosition(3.0f, 5.0f, 2.9f);
		sphere->GetRendererable().SetModel(Core::Find<Model>("Box"));
		sphere->GetRendererable().SetMaterial(0, Core::Find<SharedMaterial>("AluminiumInsulator"));
	}
	{
		auto sphere = Core::CreateInstance<GameObject>("Furry");
		sphere->GetTransform().SetPosition(2.0f, 5.0f, -3.f);
		sphere->GetTransform().SetScale(3.0f, 3.0f, 3.f);
		sphere->GetRendererable().SetModel(Core::Find<Model>("torus"));
		sphere->GetRendererable().SetMaterial(0, Core::Find<SharedMaterial>("FurTest"));
		sphere->GetRendererable().SetShaderState(0, Core::Find<ShaderState>("FurMesh"));
	}
	

	//auto light = Core::CreateInstance<GameObject>("Light");
	//light->GetTransform().SetPosition(8.2f, 11.1f, -8.9f);
	//light->GetTransform().SetRotation(45.0f, 0.0f, 0.0f);
	//light->AddComponent<SpotLight>()->Awake();

	m_MainCam = Core::CreateInstance<Camera>();
	m_MainCam->GetTransform().SetPosition(-2.6f, 11.3f, -8.6f);
	m_MainCam->GetTransform().SetRotation(40.0f, 30.0f, 0.0f);
	m_MainCam->SetProjectionValues(
		90.0f, 
		Engine::Get().GetGraphics().GetWindowWidth() / Engine::Get().GetGraphics().GetWindowHeight(), 
		0.1f, 
		1000.0f);
	m_MainCam->AddComponent<CamMove>();

	s_IsInitialized = true;
}

void Scene::ProcessGameObjectTable()
{
	auto table = Importer::LoadCSV("Data/CSV/Scene_GameObject.csv");
	size_t rowcount = table["Name"].size();
		for (int i = 0; i < rowcount; i++) {
		auto gameObject = Core::CreateInstance<GameObject>(table["Name"][i]);

		auto splitted = Importer::SplitString(table["Position"][i], '/');
		gameObject->GetTransform().SetPosition(std::stof(splitted[0]), std::stof(splitted[1]), std::stof(splitted[2]));

		splitted = Importer::SplitString(table["Rotation"][i], '/');
		gameObject->GetTransform().SetRotation(std::stof(splitted[0]), std::stof(splitted[1]), std::stof(splitted[2]));

		splitted = Importer::SplitString(table["Scale"][i], '/');
		gameObject->GetTransform().SetScale(std::stof(splitted[0]), std::stof(splitted[1]), std::stof(splitted[2]));

		if (table["TransformParent"][i] != "null") {
			gameObject->GetTransform().SetParent(Core::Find<GameObject>(table["TransformParent"][i])->GetTransformPtr());
		}

		if (table["Model"][i] != "null") {
			gameObject->GetRendererable().SetModel(Core::Find<Model>(table["Model"][i]));
		}

		if (table["AddComponent"][i] != "null") {
			splitted = Importer::SplitString(table["AddComponent"][i], '/');
			for (auto& str : splitted) {		//HardCoding
				if (str == "") continue;
				else if (str == "CharacterMove") gameObject->AddComponent<CharacterMove>();
				else if (str == "Animator") gameObject->GetRendererable().Anim = gameObject->AddComponent<Animator>();
				else if (str == "SpotLight") gameObject->AddComponent<SpotLight>();

			}
		}
	}

	Core::Pool<GameObject>::GetInstance().ForEach(AwakeGameObject);
}

void Scene::AwakeGameObject(const std::shared_ptr<GameObject> & obj) {
	for (auto weakPtr : obj->m_Components) {
		if (auto component = weakPtr.lock()) {
			component->Awake();
		}
	}
}

void Scene::Update()
{
	m_WorldTransform->UpdateMatrix(DirectX::XMMatrixIdentity(), DirectX::XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f));
	m_MainCam->UpdateView();
}

void Scene::OnGui(const char* option)
{
	for (auto child : m_WorldTransform->m_Children)
	{
		ProcessGuiHirarchy(child);
	}
	ImGui::Spacing();
}

void Scene::ProcessGuiHirarchy(std::weak_ptr<Transform> ptr)
{
	if (ptr.expired()) return;

	auto tf = ptr.lock();
	bool check = false;
	if(auto selected = m_GuiSelectedObj.lock())
	{
		check = selected->GetId() == tf->GetId();
	}

	ImGuiTreeNodeFlags nodeFlags =
		ImGuiTreeNodeFlags_OpenOnArrow
		| ImGuiTreeNodeFlags_OpenOnDoubleClick
		| (check ? ImGuiTreeNodeFlags_Selected : 0)
		| (tf->GetChildNum() == 0 ? ImGuiTreeNodeFlags_Leaf : 0)
		| ImGuiTreeNodeFlags_DefaultOpen;

	bool isNodeOpen = ImGui::TreeNodeEx(tf->GetGameObject()->Name.c_str(), nodeFlags);
	if (ImGui::IsItemClicked())
	{
		m_GuiSelectedObj = ptr;
	}

	if (isNodeOpen)
	{
		for (auto child : tf->m_Children)
		{
			ProcessGuiHirarchy(child);
		}
		ImGui::TreePop();
	}
}