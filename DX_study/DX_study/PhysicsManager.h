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
struct TimeInfo;

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

<<<<<<< HEAD:DX_study/DX_study/Engine/PhysicsManager.h
class PhysicsModule {
=======


class PhysicsManager {
>>>>>>> parent of cb3481a... refactoring:DX_study/DX_study/PhysicsManager.h
	friend class Engine;
public:
	~PhysicsModule();

<<<<<<< HEAD:DX_study/DX_study/Engine/PhysicsManager.h
	void RegisterComponent(const std::shared_ptr<Collider_v2>& compo);
	void DeregisterComponent(const std::shared_ptr<Collider_v2>& compo);

	bool Raycast(DirectX::XMFLOAT3 & from, DirectX::XMFLOAT3 & to);
	bool Raycast(DirectX::XMFLOAT3 & from, DirectX::XMFLOAT3 & to, RaycastResult & rayInfo);
	bool RaycastAll(DirectX::XMFLOAT3 & from, DirectX::XMFLOAT3 & to, std::list<RaycastResult> & rayInfo);
=======
private:
	PhysicsManager(
		Engine * const engine_ptr, 
		TimeInfo * const timeInfo, 
		std::vector<std::shared_ptr<Collider_v2>> * buffer2
	) :
		engine(engine_ptr), 
		Time(timeInfo), 
		collider_v2Buffer(buffer2){}
>>>>>>> parent of cb3481a... refactoring:DX_study/DX_study/PhysicsManager.h

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

<<<<<<< HEAD:DX_study/DX_study/Engine/PhysicsManager.h
	std::vector<std::shared_ptr<Collider_v2>> m_Colliders;

	reactphysics3d::DynamicsWorld* m_World;
	std::map< reactphysics3d::CollisionBody *, Collider_v2*> m_ColliderMap;
	reactphysics3d::Vector3* m_Gravity;
=======
	//std::vector<std::shared_ptr<Collider>> * const colliderBuffer;
	std::vector<std::shared_ptr<Collider_v2>> * const collider_v2Buffer;
	Engine * const engine;
	TimeInfo * const Time;

	reactphysics3d::DynamicsWorld* mReactPhysics_DYNAMIC_WORLD;
	std::map< reactphysics3d::CollisionBody *, Collider_v2*> mBody_Collider_MAP;
	reactphysics3d::Vector3* mGravity;
>>>>>>> parent of cb3481a... refactoring:DX_study/DX_study/PhysicsManager.h
};