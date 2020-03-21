#include "Physics.h"
#include "Engine/Engine.h"
#include "Engine/PhysicsManager.h"

bool Physics::Raycast(DirectX::XMFLOAT3 & from, DirectX::XMFLOAT3 & to)
{
	auto& pm = Engine::GetInstance().GetPhysicsManager();
	return pm.Raycast(from, to);
}

bool Physics::Raycast(DirectX::XMFLOAT3 & from, DirectX::XMFLOAT3 & to, RaycastResult & rayInfo)
{
	auto& pm = Engine::GetInstance().GetPhysicsManager();
	return pm.Raycast(from, to, rayInfo);
}