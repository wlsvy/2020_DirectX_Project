#include "SceneManager.h"
#include "Engine.h"
#include "ModuleResource.h"
#include "../CustomScript/CustomScriptInclude.h"
#include "../Graphics/Model.h"
#include "../Graphics/Shaders.h"
#include "../Graphics/Graphics.h"
#include "../UI/imGui/imgui.h"

#include <crtdbg.h>
#if _DEBUG
#define new new(_NORMAL_BLOCK, __FILE__, __LINE__)
#define malloc(s) _malloc_dbg(s, _NORMAL_BLOCK, __FILE__, __LINE__)
#endif

using namespace DirectX;
//여기에 커스텀 스크립트 헤더 include

SceneManager::SceneManager() :
	m_RootGameObject(new GameObject_v2())
{
	Transform * worldTransform = new Transform(null);

	mWorldTransform = worldTransform;
	mWorldMatrix = DirectX::XMMatrixScaling(1.0f, 1.0f, 1.0f)
		* DirectX::XMMatrixRotationRollPitchYaw(0.0f, 0.0f ,0.0f)
		* DirectX::XMMatrixTranslation(0.0f, 0.0f, 0.0f);
}

void SceneManager::Custom_Test_Obj_Set()
{
	std::string cat_filepath = "Data\\Objects\\Test_cat\\12221_Cat_v1_l3.obj";
	std::string nanosuit_filepath = "Data\\Objects\\nanosuit\\nanosuit.obj";
	std::string tmpName = "";

	GAMEOBJECT_INIT_DESC desc;
	desc.pos = XMFLOAT3(0.0f, -30.0f, 0.0f);
	desc.rot = XMFLOAT3(0.0f, 0.0f, 0.0f);
	desc.scale = DirectX::XMFLOAT3(0.3f, 0.3f, 0.3f);
	desc.model = modelBuffer.buffer[9];
	desc.obj_name = "first Created";
	desc.vshaderPtr = Module::GetGraphicsModule().GetVshader("skinned_vertex");
	desc.pshaderPtr = Module::GetGraphicsModule().GetPshader("pixelshader");

	GameObject_v2 *gameObj = AddGameObject(desc);
	gameObj->AddComponent<Animator>();
	gameObj->GetComponent<Animator>()->SetAnimClip(&Engine::GetInstance().GetAnimClipBuffer()->at(0));
	

	desc.model = modelBuffer.buffer[7];
	desc.vshaderPtr = Module::GetGraphicsModule().GetVshader("vertexshader");
	desc.pshaderPtr = Module::GetGraphicsModule().GetPshader("pixelshader");
	desc.scale = DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f);
	GameObject_v2 * tmp;
	for (int i = 0; i < 4; i++) {
		desc.pos = XMFLOAT3(-20.0f * i, 0.0f, 10.0f);
		desc.obj_name = "Nano Suit" + std::to_string(i);
		gameObj = AddGameObject(desc);
		gameObj->AddComponent<cstest>();
		gameObj->AddComponent<cstest02>();

		if (i != 0)
			gameObj->transform.SetParent(&tmp->transform);

		tmp = gameObj;
	}

	//Simple Geometry Array
	desc.pshaderPtr = Module::GetGraphicsModule().GetPshader("PS_Camera");
	for (int i = 1; i < 6; i++) {
		desc.pos = XMFLOAT3(20.0f + i * 5.0f, 5.0f, 10.0f);
		desc.model = modelBuffer.buffer[i];
		desc.obj_name = "Simple Geometry" + std::to_string(i);
		gameObj = AddGameObject(desc);
		gameObj->AddComponent<SphereCollider_ver2>();
	}

	//billboard test
	desc.vshaderPtr = Module::GetGraphicsModule().GetVshader("vertexshader_geo");
	desc.pshaderPtr = Module::GetGraphicsModule().GetPshader("pixelshader");
	desc.gshaderPtr = Module::GetGraphicsModule().GetGshader("BillBoardSample");
	desc.pos = XMFLOAT3(0, 0.0f, 5.0f);
	desc.model = modelBuffer.buffer[0];
	desc.obj_name = "BillBoard Test";
	gameObj = AddGameObject(desc);
	gameObj->AddComponent<SphereCollider_ver2>();
	desc.vshaderPtr = Module::GetGraphicsModule().GetVshader("vertexshader");
	desc.pshaderPtr = Module::GetGraphicsModule().GetPshader("pixelshader");
	desc.gshaderPtr = Module::GetGraphicsModule().GetGshader("");

	//billboard test 2
	/*desc.gshaderID = 0;
	desc.vshaderID = 3;
	desc.pos = XMFLOAT3(0.0f, 0.0f, 0.0f);
	desc.model = modelBuffer.buffer[1];
	desc.obj_name = "BillBoard Test box";
	gameObj = AddGameObject(desc);
	gameObj->AddComponent<BoxCollider_ver2>();
	desc.gshaderID = -1;
	desc.vshaderID = 1;*/

	//Character Controller
	desc.model = modelBuffer.buffer[3];
	desc.pos = XMFLOAT3(15.0f, 20.0f, 10.0f);
	desc.obj_name = "Character Controller";
	gameObj = AddGameObject(desc);
	gameObj->AddComponent<CharacterController>();
	gameObj->AddComponent<GroundTest>();

	//RigidBody
	desc.model = modelBuffer.buffer[1];
	desc.pos = XMFLOAT3(23.3f, 15.0f, 10.0f);
	desc.rot = XMFLOAT3(0.0f, 0.0f, 0.0f);
	desc.obj_name = "GameObject";
	gameObj = AddGameObject(desc);
	gameObj->AddComponent<BoxRigidBody>();

	//RigidBody
	desc.pos = XMFLOAT3(18.3f, 19.0f, 10.0f);
	desc.rot = XMFLOAT3(0.0f, 0.0f, 0.0f);
	gameObj = AddGameObject(desc);
	gameObj->AddComponent<SphereRigidBody>();

	//floor
	desc.pos = XMFLOAT3(20.0f, -0.0f, 10.0f);
	desc.rot = XMFLOAT3(0.0f, 0.0f, 0.0f);
	desc.scale = XMFLOAT3(15.0f, 1.0f, 15.0f);
	gameObj = AddGameObject(desc);
	gameObj->AddComponent<BoxCollider_ver2>();

	//wall
	desc.pos = XMFLOAT3(30.0f, -0.0f, 10.0f);
	desc.rot = XMFLOAT3(45.0f, 0.0f, 0.0f);
	desc.scale = XMFLOAT3(15.0f, 1.0f, 15.0f);
	gameObj = AddGameObject(desc);
	gameObj->AddComponent<BoxCollider_ver2>();

	//terrain
	/*desc.pshaderID = 7;
	desc.model = modelBuffer.buffer[2];
	desc.pos = XMFLOAT3(0.0f, -1.0f, 0.0f);
	desc.scale = XMFLOAT3(1.0f, 1.0f, 1.0f);
	gameObj = AddGameObject(desc);
	gameObj->AddComponent<Terrain>();
	gameObj->GetComponent<Terrain>()->heightFilePath = "Data\\Textures\\heightmap01.bmp";*/

	//Light Obj
	desc.pshaderPtr = Module::GetGraphicsModule().GetPshader("pixelshader_nolight");
	desc.model = modelBuffer.buffer[6];
	desc.pos = XMFLOAT3(0.0f, 0.0f, 0.0f);
	desc.rot = XMFLOAT3(0.0f, 0.0f, 0.0f);
	desc.scale = XMFLOAT3(1.0f, 1.0f, 1.0f);
	gameObj = AddGameObject(desc);
	gameObj->AddComponent<DirectionalLight>();
}

bool SceneManager::DestoryGameObject(GameObject_v2& gameObject)
{
	for (auto iter = m_GameObjects.begin(); iter != m_GameObjects.end(); iter++) {
		if (gameObject == (**iter)) {
			m_GameObjects.erase(iter);
			return true;
		}
	}

	return false;
}

void SceneManager::OnGui()
{
	ImGuiTreeNodeFlags node_flags = ImGuiTreeNodeFlags_CollapsingHeader
		| ImGuiTreeNodeFlags_OpenOnArrow
		| ImGuiTreeNodeFlags_OpenOnDoubleClick
		| ImGuiTreeNodeFlags_DefaultOpen;

	if (ImGui::CollapsingHeader("GameObject", node_flags))
	{
		ImGui::Spacing();

		for (auto child : mWorldTransform->mChildTransform)
		{
			UIrecursiveTransformCheck(child);
		}
	}
	ImGui::Spacing();
}

void SceneManager::UIrecursiveTransformCheck(Transform * transform)
{
	ImGuiTreeNodeFlags node_flags = ImGuiTreeNodeFlags_OpenOnArrow
		| ImGuiTreeNodeFlags_OpenOnDoubleClick
		| ((mUI_Selected_Transform_ID == transform->getComponentID()) ? ImGuiTreeNodeFlags_Selected : 0)
		| ((transform->GetChildNum() == 0) ? ImGuiTreeNodeFlags_Leaf : 0)
		| ImGuiTreeNodeFlags_DefaultOpen;

	bool node_open = ImGui::TreeNodeEx(transform->GameObject.Name, node_flags);
	if (ImGui::IsItemClicked())
	{
		mUI_Selected_Transform_ID = transform->getComponentID();
		mUI_Selectd_Transform_Ptr = transform;
	}

	if (node_open)
	{
		for (auto child : transform->mChildTransform)
		{
			UIrecursiveTransformCheck(child);
		}
		ImGui::TreePop();
	}
}

Model * SceneManager::getModelByName(const std::string & _str)
{
	auto iter = Module::GetGraphicsModule().mModelMap.find(_str);
	if (iter != Module::GetGraphicsModule().mModelMap.end()) {
		return iter->second;
	}
	return nullptr;
}

GameObject_v2 * SceneManager::getUIselectedObj()
{
	if (mUI_Selectd_Transform_Ptr == nullptr) return nullptr;
	return &mUI_Selectd_Transform_Ptr->GameObject;
}

SceneManager::~SceneManager()
{
	while (mWorldTransform->mChildTransform.size() != 0) {
		mWorldTransform->mChildTransform[0]->GameObject.Destroy();
	}

	delete mWorldTransform;
}

GameObject_v2* SceneManager::AddGameObject(GAMEOBJECT_INIT_DESC & desc)
{
	desc.obj_id = ObjIDcontributor++;

	GameObject_v2 * ptr = new GameObject_v2(desc);
	m_GameObjects.emplace_back(ptr);
	ptr->transform.m_ID = ComponentIDcontributor++;
	ptr->transform.mWorldTransform = mWorldTransform;
	ptr->transform.SetParent(mWorldTransform);
	ptr->renderer.m_ID = ComponentIDcontributor++;

	return ptr;
}

void SceneManager::Update()
{
	for (auto child : mWorldTransform->mChildTransform) {
		RecursiveTransformUpdate(child, mWorldMatrix);
	}
}

void SceneManager::RecursiveTransformUpdate(Transform * transform, DirectX::XMMATRIX & _parentMatrix)
{
	transform->UpdateMatrix(_parentMatrix);
	DirectX::XMMATRIX parentMat = transform->GetWorldMatrix();

	for (auto child : transform->mChildTransform) {
		RecursiveTransformUpdate(child, parentMat);
	}
}

ModelBuffer::~ModelBuffer()
{
	for (std::vector<Model*>::iterator it = buffer.begin(); it != buffer.end(); it++)
		delete *it;
}

TerrainModelBuffer::~TerrainModelBuffer()
{
	for (std::vector<Model*>::iterator it = buffer.begin(); it != buffer.end(); it++)
		delete *it;
}
