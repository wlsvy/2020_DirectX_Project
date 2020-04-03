#include "Scene.h"
#include "../../Component/Transform.h"

Scene::Scene() : m_WorldTransform(std::make_shared<Transform>(nullptr))
{
}

void Scene::UpdateTransform()
{
	m_WorldTransform->UpdateMatrix(DirectX::XMMatrixIdentity());
}

std::shared_ptr<Transform> Scene::GetWorldTransform()
{
	return m_WorldTransform;
}
