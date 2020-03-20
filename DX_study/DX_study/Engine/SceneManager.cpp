#include "SceneManager.h"
#include "Engine.h"
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

SceneManager::SceneManager(PhysicsManager * const _physcisManager) : mPhysicsManager(_physcisManager)
{
	COMPONENT_INIT_DESC desc;
	Transform * worldTransform = new Transform(desc);

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
	desc.scene_manager = this;
	desc.vshaderPtr = getVshaderByName("skinned_vertex");
	desc.pshaderPtr = getPshaderByName("pixelshader");

	GameObject_v2 *gameObj = AddGameObject(desc);
	gameObj->AddComponent<Animator>();
	gameObj->GetComponent<Animator>()->SetAnimClip(&Engine::GetInstance().GetAnimClipBuffer()->at(0));
	

	desc.model = modelBuffer.buffer[7];
	desc.vshaderPtr = getVshaderByName("vertexshader");
	desc.pshaderPtr = getPshaderByName("pixelshader");
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
	desc.pshaderPtr = getPshaderByName("PS_Camera");
	for (int i = 1; i < 6; i++) {
		desc.pos = XMFLOAT3(20.0f + i * 5.0f, 5.0f, 10.0f);
		desc.model = modelBuffer.buffer[i];
		desc.obj_name = "Simple Geometry" + std::to_string(i);
		gameObj = AddGameObject(desc);
		gameObj->AddComponent<SphereCollider_ver2>();
	}

	//billboard test
	desc.vshaderPtr = getVshaderByName("vertexshader_geo");
	desc.pshaderPtr = getPshaderByName("pixelshader");
	desc.gshaderPtr = getGshaderByName("BillBoardSample");
	desc.pos = XMFLOAT3(0, 0.0f, 5.0f);
	desc.model = modelBuffer.buffer[0];
	desc.obj_name = "BillBoard Test";
	gameObj = AddGameObject(desc);
	gameObj->AddComponent<SphereCollider_ver2>();
	desc.vshaderPtr = getVshaderByName("vertexshader");
	desc.pshaderPtr = getPshaderByName("pixelshader");
	desc.gshaderPtr = getGshaderByName("");

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
	desc.pshaderPtr = getPshaderByName("pixelshader_nolight");
	desc.model = modelBuffer.buffer[6];
	desc.pos = XMFLOAT3(0.0f, 0.0f, 0.0f);
	desc.rot = XMFLOAT3(0.0f, 0.0f, 0.0f);
	desc.scale = XMFLOAT3(1.0f, 1.0f, 1.0f);
	gameObj = AddGameObject(desc);
	gameObj->AddComponent<DirectionalLight>();
}

void SceneManager::ClassifyComponent(Component * _component, GameObject_v2 * _destination)
{
	if (_component->mComponentID != -1) {
		MessageBoxA(NULL, "This Component isn't new Component", "Error", MB_ICONERROR);
		return;
	}
	_component->mComponentID = ComponentIDcontributor++;

	ScriptBehaviour *scriptCompo_test = dynamic_cast<ScriptBehaviour*>(_component);
	if (scriptCompo_test != NULL) {
		Engine::GetInstance().InsertScriptComponent(scriptCompo_test, _destination);
		return;
	}

	Light_ver2 *lightCompo_test = dynamic_cast<Light_ver2*>(_component);
	if (lightCompo_test != NULL) {
		Engine::GetInstance().InsertLightComponent(lightCompo_test, _destination);
		return;
	}

	Terrain *terrain_test = dynamic_cast<Terrain*>(_component);
	if (terrain_test != NULL) {
		Engine::GetInstance().InsertTerrainComponent(terrain_test, _destination);
		return;
	}

	Collider_v2 *colliderv2_test = dynamic_cast<Collider_v2*>(_component);
	if (colliderv2_test != NULL) {
		Engine::GetInstance().InsertCollider_v2Component(colliderv2_test, _destination);
		return;
	}

	Animator *animator_test = dynamic_cast<Animator*>(_component);
	if (animator_test != NULL) {
		Engine::GetInstance().InsertAnimatorComponent(animator_test, _destination);
		return;
	}

	assert("this component is impossible to classfiy." && 1 == 0);

}

void SceneManager::Component_Valid_Test()
{
	Engine::GetInstance().Component_Valid_Test();
}

bool SceneManager::Destory_GameObject(GameObject_v2 * _gameObject)
{
	for (auto iter = gameObjectBuffer.begin(); iter != gameObjectBuffer.end(); iter++) {
		if (_gameObject == iter->get()) {
			gameObjectBuffer.erase(iter);
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

void SceneManager::UIrecursiveTransformCheck(Transform * _transform)
{
	ImGuiTreeNodeFlags node_flags = ImGuiTreeNodeFlags_OpenOnArrow
		| ImGuiTreeNodeFlags_OpenOnDoubleClick
		| ((mUI_Selected_Transform_ID == _transform->getComponentID()) ? ImGuiTreeNodeFlags_Selected : 0)
		| ((_transform->GetChildNum() == 0) ? ImGuiTreeNodeFlags_Leaf : 0)
		| ImGuiTreeNodeFlags_DefaultOpen;

	bool node_open = ImGui::TreeNodeEx(_transform->gameObject->mGameObjectName, node_flags);
	if (ImGui::IsItemClicked())
	{
		mUI_Selected_Transform_ID = _transform->getComponentID();
		mUI_Selectd_Transform_Ptr = _transform;
	}

	if (node_open)
	{
		for (auto child : _transform->mChildTransform)
		{
			UIrecursiveTransformCheck(child);
		}
		ImGui::TreePop();
	}
}

VertexShader * SceneManager::getVshaderByName(const std::string & _str)
{
	auto iter = GraphicsManager::GetInstance().mVShaderMap.find(_str);
	if (iter != GraphicsManager::GetInstance().mVShaderMap.end()) {
		return iter->second;
	}

	return nullptr;
}

PixelShader * SceneManager::getPshaderByName(const std::string & _str)
{
	auto iter = GraphicsManager::GetInstance().mPShaderMap.find(_str);
	if (iter != GraphicsManager::GetInstance().mPShaderMap.end()) {
		return iter->second;
	}
	return nullptr;
}

GeometryShader * SceneManager::getGshaderByName(const std::string & _str)
{
	auto iter = GraphicsManager::GetInstance().mGShaderMap.find(_str);
	if (iter != GraphicsManager::GetInstance().mGShaderMap.end()) {
		return iter->second;
	}
	return nullptr;
}

Model * SceneManager::getModelByName(const std::string & _str)
{
	auto iter = GraphicsManager::GetInstance().mModelMap.find(_str);
	if (iter != GraphicsManager::GetInstance().mModelMap.end()) {
		return iter->second;
	}
	return nullptr;
}

GameObject_v2 * SceneManager::getUIselectedObj()
{
	if (mUI_Selectd_Transform_Ptr == nullptr) return nullptr;
	return mUI_Selectd_Transform_Ptr->gameObject;
}

PhysicsManager * SceneManager::getPhysicsManagerPtr()
{
	return mPhysicsManager;
}

GraphicsManager * SceneManager::getGraphicsManagerPtr()
{
	return &GraphicsManager::GetInstance();
}

ID3D11Device * SceneManager::getDevicePtr()
{
	return GraphicsManager::GetInstance().device.Get();
}

ID3D11DeviceContext * SceneManager::getDeviceContextPtr()
{
	return GraphicsManager::GetInstance().deviceContext.Get();
}


SceneManager::~SceneManager()
{
	while (mWorldTransform->mChildTransform.size() != 0) {
		mWorldTransform->mChildTransform[0]->gameObject->Destroy();
	}

	delete mWorldTransform;
}

GameObject_v2* SceneManager::AddGameObject(GAMEOBJECT_INIT_DESC & desc)
{
	desc.obj_id = ObjIDcontributor++;

	GameObject_v2 * ptr = new GameObject_v2(desc);
	gameObjectBuffer.emplace_back(ptr);
	ptr->transform.mComponentID = ComponentIDcontributor++;
	ptr->transform.mWorldTransform = mWorldTransform;
	ptr->transform.SetParent(mWorldTransform);
	ptr->renderer.mComponentID = ComponentIDcontributor++;

	return ptr;
}

void SceneManager::Update()
{
	for (auto child : mWorldTransform->mChildTransform) {
		RecursiveTransformUpdate(child, mWorldMatrix);
	}
}

void SceneManager::RecursiveTransformUpdate(Transform * _transform, DirectX::XMMATRIX & _parentMatrix)
{
	_transform->UpdateMatrix(_parentMatrix);
	DirectX::XMMATRIX parentMat = _transform->GetWorldMatrix();

	for (auto child : _transform->mChildTransform) {
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
