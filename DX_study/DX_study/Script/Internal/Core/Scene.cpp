#include "Scene.h"
#include "../../Component/Transform.h"

#include "../Graphics/imGui/imgui.h"
#include "../Graphics/imGui/imgui_impl_win32.h"
#include "../Graphics/imGui/imgui_impl_dx11.h"

#include "../Engine/Engine.h"
#include "../Graphics/Graphics.h"
#include "GameObject.h"
#include "../../Component/Transform.h"
#include "../../GameObject/Light.h"

Scene::Scene() : m_WorldTransform(std::make_shared<Transform>(nullptr, "World Transform"))
{
}

void Scene::Update()
{
	m_WorldTransform->UpdateMatrix(DirectX::XMMatrixIdentity());
}

void Scene::OnGui()
{
	Transform& tf = Engine::Get().GetGraphics().light->GetTransform();
	ImGui::Begin("Light Transform");
	ImGui::DragFloat3("Pos", &tf.position.x, 0.1f, -1000.0f, 1000.0f);
	ImGui::DragFloat3("Rot", &tf.rotation.x, 0.1f, -1000.0f, 1000.0f);
	ImGui::End();
}

std::shared_ptr<Transform> Scene::GetWorldTransform()
{
	return m_WorldTransform;
}
