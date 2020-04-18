#include "Scene.h"
#include "../../Component/Transform.h"

#include "../Graphics/imGui/imgui.h"
#include "../Graphics/imGui/imgui_impl_win32.h"
#include "../Graphics/imGui/imgui_impl_dx11.h"
#include "../Graphics/Graphics.h"
#include "../Graphics/Model.h"
#include "../Graphics/Shaders.h"
#include "../Graphics/AnimationClip.h"

#include "GameObject.h"
#include "ImportHelper.h"
#include "../Engine/Engine.h"
#include "../../Component/Transform.h"
#include "../../Component/Renderable.h"
#include "../../Component/Custom/CamMove.h"
#include "../../Component/Custom/CharacterMove.h"
#include "../../Component/Animator.h"
#include "../../GameObject/Camera.h"
#include "../../GameObject/Light.h"
#include "../../GameObject/Sprite.h"

bool Scene::s_IsInitialized = false;

Scene::Scene() : m_WorldTransform(std::make_shared<Transform>(nullptr, "World Transform"))
{
}

void Scene::Initialize()
{
	ProcessGameObjectTable();
	Pool::Find<GameObject>("X_Bot")->GetRenderer().Anim->SetClip(Pool::Find<AnimationClip>("X_Bot_Idle"));
	Pool::Find<GameObject>("X_Bot")->GetRenderer().Anim->Play();

	/*auto gameObject = Pool::CreateInstance<GameObject>("Y Boy");
	gameObject->GetTransform().SetPosition(0.0f, 0.0f, 0.0f);
	gameObject->GetTransform().SetScale(0.1f, 0.1f, 0.1f);
	gameObject->GetRenderer().SkinnedModel = Pool::Find<SkinnedModel>("Y_Bot");
	gameObject->GetRenderer().Anim = gameObject->AddComponent<Animator>();
	gameObject->GetRenderer().Anim->Clip = Pool::Find<AnimationClip>("Y_Bot_Idle");
	gameObject->GetRenderer().Anim->Play();
	gameObject->AddComponent<CharacterMove>()->Init();
	gameObject->GetRenderer().Vshader = Pool::Find<VertexShader>("skinned_vertex");
	gameObject->GetRenderer().Pshader = Pool::Find<PixelShader>("pixelshader_deferred");*/

	/*for (int i = 0; i < 5; i++) {
		for (int j = 0; j < 5; j++) {
			for (int k = 0; k < 5; k++) {
				auto tempObj = Pool::CreateInstance<GameObject>("BoxTest" + std::to_string(i) + std::to_string(j) + std::to_string(k));
				tempObj->GetTransform().SetPosition(5.0f + i * 3.0f, j * 3.0f, k * 3.0f);
				tempObj->GetTransform().SetParent(gameObject->GetTransformPtr());
				tempObj->GetRenderer().Model = Pool::Find<Model>("Box");
				tempObj->GetRenderer().Vshader = Pool::Find<VertexShader>("vertexshader");
				tempObj->GetRenderer().Pshader = Pool::Find<PixelShader>("pixelshader_deferred");
			}
		}
	}*/

	auto light = Pool::CreateInstance<Light>();
	light->GetRenderer().Model = Pool::Find<Model>("light");
	light->GetRenderer().Vshader = Pool::Find<VertexShader>("vertexshader");
	light->GetRenderer().Pshader = Pool::Find<PixelShader>("pixelshader_deferred");

	m_MainCam = Pool::CreateInstance<Camera>();
	m_MainCam->GetTransform().SetPosition(0.0f, 0.0f, -2.0f);
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
	for (int i = 0; i < table["Name"].size(); i++) {
		auto gameObject = Pool::CreateInstance<GameObject>(table["Name"][i]);

		auto splitted = Importer::SplitString(table["Position"][i], '/');
		gameObject->GetTransform().SetPosition(std::stof(splitted[0]), std::stof(splitted[1]), std::stof(splitted[2]));

		splitted = Importer::SplitString(table["Rotation"][i], '/');
		gameObject->GetTransform().SetRotation(std::stof(splitted[0]), std::stof(splitted[1]), std::stof(splitted[2]));

		splitted = Importer::SplitString(table["Scale"][i], '/');
		gameObject->GetTransform().SetScale(std::stof(splitted[0]), std::stof(splitted[1]), std::stof(splitted[2]));

		if (table["TransformParent"][i] != "null") {
			gameObject->GetTransform().SetParent(Pool::Find<GameObject>(table["TransformParent"][i])->GetTransformPtr());
		}

		gameObject->GetRenderer().Vshader = Pool::Find<VertexShader>(table["VertexShader"][i]);
		gameObject->GetRenderer().Pshader = Pool::Find<PixelShader>(table["PixelShader"][i]);

		if (table["Model"][i] != "null") {
			gameObject->GetRenderer().Model = Pool::Find<Model>(table["Model"][i]);
		}
		if (table["SkinnedModel"][i] != "null") {
			gameObject->GetRenderer().SkinnedModel = Pool::Find<SkinnedModel>(table["SkinnedModel"][i]);
		}

		splitted = Importer::SplitString(table["AddComponent"][i], '/');
		for (auto& str : splitted) {
			if (str == "") continue;
			else if (str == "CharacterMove") gameObject->AddComponent<CharacterMove>();
			else if (str == "Animator") gameObject->GetRenderer().Anim = gameObject->AddComponent<Animator>();
		}
	}

	Pool::ObjectPool<GameObject>::GetInstance().ForEach(AwakeGameObject);
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
	m_WorldTransform->UpdateMatrix(DirectX::XMMatrixIdentity());
	m_MainCam->UpdateViewMatrix();
}

void Scene::OnGui()
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