#pragma once
#include <vector>
#include <list>
#include <map>
#include <memory>
#include <DirectXMath.h>

class Engine; //include 정의 조심
class GameObject_v2;
class Component;
class Collider_v2;

namespace DirectX {
	struct XMFLOAT3;
}

namespace reactphysics3d {
	class DynamicsWorld;
	class CollisionBody;
	struct Vector3;
}

struct RaycastResult {
	DirectX::XMFLOAT3 mWorldPoint;
	DirectX::XMFLOAT3 mWorldNormal;
	float mHitFraction;
	Collider_v2 * mCollider;
};

class PhysicsModule {
	friend class Engine;
public:
	~PhysicsModule();

	void RegisterComponent(const std::shared_ptr<Collider_v2>& compo);
	void DeregisterComponent(const std::shared_ptr<Collider_v2>& compo);

	bool Raycast(DirectX::XMFLOAT3 & from, DirectX::XMFLOAT3 & to);
	bool Raycast(DirectX::XMFLOAT3 & from, DirectX::XMFLOAT3 & to, RaycastResult & rayInfo);
	bool RaycastAll(DirectX::XMFLOAT3 & from, DirectX::XMFLOAT3 & to, std::list<RaycastResult> & rayInfo);

private:
	PhysicsModule() {}

	bool Initialize();
	void PhysicsCompoInit(std::shared_ptr<Collider_v2> _component);
	void Update();
	void PreUpdate();
	void PhysicsUpdate();
	void UpdateCollider();
	void UpdateComponent();
	void CollisionTest();
	void CollisionTest_ver2();
	void react3DCollisioinTest();
	void react3DCollisioinTest(Collider_v2* _collider);

	std::vector<std::shared_ptr<Collider_v2>> m_Colliders;

	reactphysics3d::DynamicsWorld* m_World;
	std::map< reactphysics3d::CollisionBody *, Collider_v2*> m_ColliderMap;
	reactphysics3d::Vector3* m_Gravity;
};