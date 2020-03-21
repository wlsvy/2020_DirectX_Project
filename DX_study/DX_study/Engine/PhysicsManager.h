#pragma once
#include <vector>
#include <list>
#include <map>
#include <memory>
#include <DirectXMath.h>

class Engine; //include ���� ����
class GameObject_v2;
class Component;
class Collider;
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
	bool Raycast(DirectX::XMFLOAT3 & from, DirectX::XMFLOAT3 & to);
	bool Raycast(DirectX::XMFLOAT3 & from, DirectX::XMFLOAT3 & to, RaycastResult & rayInfo);
	bool RaycastAll(DirectX::XMFLOAT3 & from, DirectX::XMFLOAT3 & to, std::list<RaycastResult> & rayInfo);

private:
	PhysicsModule(
		std::vector<std::shared_ptr<Collider_v2>> * buffer2
	) :
		collider_v2Buffer(buffer2){}

	~PhysicsModule();

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

	//std::vector<std::shared_ptr<Collider>> * const colliderBuffer;
	std::vector<std::shared_ptr<Collider_v2>> * const collider_v2Buffer;

	reactphysics3d::DynamicsWorld* m_World;
	std::map< reactphysics3d::CollisionBody *, Collider_v2*> m_ColliderMap;
	reactphysics3d::Vector3* m_Gravity;
};

//namespace Physics {
//	bool Raycast(DirectX::XMFLOAT3 & _from, DirectX::XMFLOAT3 & _to);
//	bool Raycast(DirectX::XMFLOAT3 & _from, DirectX::XMFLOAT3 & _to, RaycastResult & _rayinfo);
//	bool RaycastAll(DirectX::XMFLOAT3 & _from, DirectX::XMFLOAT3 & _to, std::list<RaycastResult> & _rayinfo);
//}