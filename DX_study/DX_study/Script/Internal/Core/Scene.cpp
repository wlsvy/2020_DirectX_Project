#include "Scene.h"
#include "../../Component/Transform.h"

#include "../Graphics/imGui/imgui.h"
#include "../Graphics/imGui/imgui_impl_win32.h"
#include "../Graphics/imGui/imgui_impl_dx11.h"
#include "../Graphics/Graphics.h"
#include "../Graphics/Model.h"
#include "../Graphics/Shaders.h"
#include "../Graphics/AnimationClip.h"

#include "../Engine/Engine.h"
#include "../Graphics/Graphics.h"
#include "GameObject.h"
#include "../../Component/Transform.h"
#include "../../Component/Renderable.h"
#include "../../Component/Custom/CamMove.h"
#include "../../Component/Custom/CharacterMove.h"
#include "../../Component/Animator.h"
#include "../../GameObject/Camera.h"
#include "../../GameObject/Light.h"
#include "../../GameObject/Sprite.h"

Scene::Scene() : m_WorldTransform(std::make_shared<Transform>(nullptr, "World Transform"))
{
}

void Scene::Initialize()
{
	for (int i = 0; i < 5; i++) {
		for (int j = 0; j < 5; j++) {
			for (int k = 0; k < 5; k++) {
				auto tempObj = Pool::CreateInstance<GameObject>("BoxTest" + std::to_string(i) + std::to_string(j));
				tempObj->GetTransform().SetPosition(5.0f + i * 3.0f, j * 3.0f, k * 3.0f);
				tempObj->GetRenderer().Model = Pool::Find<Model>("Box");
				tempObj->GetRenderer().Vshader = Pool::Find<VertexShader>("vertexshader");
				tempObj->GetRenderer().Pshader = Pool::Find<PixelShader>("pixelshader_deferred");
			}
		}
	}

	auto gameObject = Pool::CreateInstance<GameObject>("Y Boy");
	gameObject->GetTransform().SetPosition(0.0f, 0.0f, 0.0f);
	gameObject->GetTransform().SetScale(0.1f, 0.1f, 0.1f);
	gameObject->GetRenderer().SkinnedModel = Pool::Find<SkinnedModel>("Y Bot");
	gameObject->GetRenderer().Anim = gameObject->AddComponent<Animator>();
	gameObject->GetRenderer().Anim->Clip = Pool::Find<AnimationClip>("Y Bot_Idle");
	gameObject->GetRenderer().Anim->Play();
	gameObject->AddComponent<CharacterMove>()->Init();
	gameObject->GetRenderer().Vshader = Pool::Find<VertexShader>("skinned_vertex");
	gameObject->GetRenderer().Pshader = Pool::Find<PixelShader>("pixelshader_deferred");

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
}

void Scene::Update()
{
	m_WorldTransform->UpdateMatrix(DirectX::XMMatrixIdentity());
	m_MainCam->UpdateViewMatrix();
}

void Scene::OnGui()
{
	static auto light = std::dynamic_pointer_cast<Light>(Pool::Find<GameObject>("Light"));
	Transform& tf = light->GetTransform();
	ImGui::Begin("Light Transform");
	ImGui::DragFloat3("Pos", &tf.position.x, 0.1f, -1000.0f, 1000.0f);
	ImGui::DragFloat3("Rot", &tf.rotation.x, 0.1f, -1000.0f, 1000.0f);
	ImGui::End();
}