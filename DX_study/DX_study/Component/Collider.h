#pragma once
//더 이상 안씀


//#include "Component.h"
//#include "../Graphics/DebugDraw.h" //이거는 가능하면 없애버려야 되는데

//namespace MyCustom {
//	enum CollisionType { COLLISION_DISJOINT = 0, COLLISION_INTERSECT = 1, COLLISION_CONTAIN = 2 };
//	enum COLLIDER { COLLIDER_AABB = 0, COLLIDER_BOX = 1, COLLIDER_SPHERE = 2 };
//}
//
//class AABBCollider;
//class BoxCollider;
//class SphereCollider;
//struct DirectX::BoundingBox;
//namespace reactphysics3d {
//	class DynamicsWorld;
//	class RigidBody;
//	class CollisionBody;
//	class DynamicsWorld;
//	class ProxyShape;
//	class BoxShape;
//	enum class BodyType;
//}
//
//struct COLLIDER_DEBUG_MODEL {
//	const DirectX::XMFLOAT3* position = nullptr;
//	const DirectX::XMFLOAT3* rotation = nullptr;
//	const DirectX::XMFLOAT3* scale = nullptr;
//
//	BYTE typeNum = -1;
//	const DirectX::BoundingBox* aabbPtr = nullptr;
//	const DirectX::BoundingOrientedBox* boxPtr = nullptr;
//	const DirectX::BoundingSphere* spherePtr = nullptr;
//	const BoxCollider * boxCollider = nullptr;
//};
//
//class Collider : public Component {
//	friend class PhysicsManager;
//public:
//	virtual MyCustom::CollisionType My_CollisionTest(const Collider * collider) abstract;
//	virtual DirectX::ContainmentType Debug_CollisionTest(const Collider * collider) abstract;
//	virtual void DebugModel_Draw(DirectX::PrimitiveBatch<DirectX::VertexPositionColor>* batch, DirectX::FXMVECTOR color) abstract;
//	virtual COLLIDER_DEBUG_MODEL Get_DebugModelType() const abstract;
//	//virtual reactphysics3d::CollisionBody *  Getreact3Dmodel();
//
//	MyCustom::COLLIDER type;
//	const COMPONENT_TYPE mType = COMPONENT_COLLISION;
//	BYTE CollisionMask = 0xff;
//
//	Transform *transform;
//protected:
//	std::vector<Collider*> collidObj; //중간에 컴포넌트 삭제해서 댕글링 만드는 경우의 수 존재. 근데 스마트 포인터로 바꾸기 귀찮다
//
//private:
//	virtual void Update() override {}
//};
//
//class AABBCollider : public Collider {
//	friend class PhysicsManager;
//public:
//	void  initialize(const COMPONENT_INIT_DESC & desc) override;
//	MyCustom::CollisionType My_CollisionTest(const Collider * collider) override;
//	DirectX::ContainmentType Debug_CollisionTest(const Collider * collider) override;
//	void DebugModel_Draw(DirectX::PrimitiveBatch<DirectX::VertexPositionColor>* batch, DirectX::FXMVECTOR color) override;
//	COLLIDER_DEBUG_MODEL Get_DebugModelType() const override;
//	
//
//	DirectX::XMFLOAT3 Center = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);
//	DirectX::XMFLOAT3 size = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);
//	DirectX::BoundingBox colliderModel;
//};
//
//class BoxCollider : public Collider {
//	friend class PhysicsManager;
//	friend class GraphicsManager;
//public:
//	void  initialize(const COMPONENT_INIT_DESC & desc) override;
//
//	MyCustom::CollisionType My_CollisionTest(const Collider * collider) override;
//	DirectX::ContainmentType Debug_CollisionTest(const Collider * collider) override;
//	void DebugModel_Draw(DirectX::PrimitiveBatch<DirectX::VertexPositionColor>* batch, DirectX::FXMVECTOR color) override;
//	COLLIDER_DEBUG_MODEL Get_DebugModelType() const override;
//
//	DirectX::XMFLOAT3 localCenter = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);
//	DirectX::XMFLOAT3 localScale = DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f);
//	DirectX::XMFLOAT3 localRotation = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);
//	DirectX::BoundingOrientedBox colliderModel;
//
//private:
//	void Update() override;
//	bool Intersect_Test(const BoxCollider* _collider);
//	
//};
//
//class SphereCollider : public Collider {
//	friend class PhysicsManager;
//public:
//	void  initialize(const COMPONENT_INIT_DESC & desc) override;
//	MyCustom::CollisionType My_CollisionTest(const Collider * collider) override;
//	DirectX::ContainmentType Debug_CollisionTest(const Collider * collider) override;
//	void DebugModel_Draw(DirectX::PrimitiveBatch<DirectX::VertexPositionColor>* batch, DirectX::FXMVECTOR color) override;
//	COLLIDER_DEBUG_MODEL Get_DebugModelType() const override;
//
//	DirectX::XMFLOAT3 center = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);;
//	float Radius = 1.0f;
//	DirectX::BoundingSphere colliderModel;
//};
