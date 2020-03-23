#include "PhysicsManager.h"
#include "../Timer.h"
#include "../Component/GameObject_v2.h"
#include "../Component/CollisionInfo.h"
#include <cassert>

//#pragma comment (lib, "reactphysics3d.lib")
#include <reactphysics3d/reactphysics3d.h>
#include <reactphysics3d/collision/ContactManifold.h>
#include <reactphysics3d/constraint/ContactPoint.h>

reactphysics3d::Vector3 Dflot3ToRvec3(const DirectX::XMFLOAT3 & _src) { return reactphysics3d::Vector3(_src.x, _src.y, _src.z); }
DirectX::XMFLOAT3 Rvec3ToDfloat3(const reactphysics3d::Vector3 & _src) { return DirectX::XMFLOAT3(_src.x, _src.y, _src.z); }

using RPvector3 = reactphysics3d::Vector3;
using RPray = reactphysics3d::Ray;

class r3dCallback : public reactphysics3d::CollisionCallback {
public:
	int aa = 0;
	void notifyContact(const CollisionCallbackInfo &collisionCallbackInfo) {
		reactphysics3d::ContactManifoldListElement * t = collisionCallbackInfo.contactManifoldElements;
		reactphysics3d::ContactManifold * maf = t->getContactManifold();
		//maf->getNext();
		reactphysics3d::ContactPoint * point = maf->getContactPoints();
		reactphysics3d::Vector3 contactpoint1 =  point->getLocalPointOnShape1();
		//collisionCallbackInfo.contactManifoldElements[0]->
		aa++;
	}
};

class r3dCollisionCallback : public reactphysics3d::CollisionCallback {
public:
	r3dCollisionCallback(Collider_v2 * _collider, std::map< reactphysics3d::CollisionBody *, Collider_v2*> * _map) :
		mCollider(_collider) ,
		mBody_Collider_MAP(_map) {}

	void ContactManifoldTraverse(reactphysics3d::ContactManifoldListElement * _maniFoldList) {
		reactphysics3d::ContactManifold * contactManifold = _maniFoldList->getContactManifold();

		while (contactManifold != nullptr) {
			Make_CollisionInfo(contactManifold);
			contactManifold = contactManifold->getNext();
			colliderNum++;
		}
	}
	void Make_CollisionInfo(reactphysics3d::ContactManifold * _contactManifold) {
		mCollider->mCollisionInfo.push_back(std::make_unique<CollisionInfo>());
		CollisionInfo * collision = mCollider->mCollisionInfo.back().get();
		auto iter = mBody_Collider_MAP->find(_contactManifold->getBody2());
		if (iter != mBody_Collider_MAP->end()) {
			collision->mCollider = iter->second;
			collision->mGameObject = &collision->mCollider->GameObject;
		}

		reactphysics3d::Vector3 contactposition;
		DirectX::XMFLOAT3 DcontactPos;
		reactphysics3d::ContactPoint * point = _contactManifold->getContactPoints();
		reactphysics3d::Vector3 centerpos = _contactManifold->getBody1()->getTransform().getPosition();
		DirectX::XMVECTOR centerVec = DirectX::XMVectorSet(centerpos.x, centerpos.y, centerpos.z, 1.0f);

		while (point != nullptr) {
			contactposition = point->getLocalPointOnShape1();
			DirectX::XMVECTOR contactVec = DirectX::XMVectorSet(contactposition.x, contactposition.y, contactposition.z, 0.0f);

			contactposition += centerpos;
			DcontactPos = DirectX::XMFLOAT3(contactposition.x, contactposition.y, contactposition.z);
			
			collision->mContactPoints.push_back(DcontactPos);
			collision->mCenterContactVec.push_back(DirectX::XMVector3Normalize(contactVec));
			point = point->getNext();
		}
	}
	void notifyContact(const CollisionCallbackInfo &collisionCallbackInfo) {
		reactphysics3d::ContactManifoldListElement * maniFoldList = collisionCallbackInfo.contactManifoldElements;
		while (maniFoldList != nullptr) {
			ContactManifoldTraverse(maniFoldList);
			maniFoldList = maniFoldList->getNext();
		}
	}

	Collider_v2 * mCollider = nullptr;
	std::map< reactphysics3d::CollisionBody *, Collider_v2*> *mBody_Collider_MAP;
	std::list<std::unique_ptr<CollisionInfo>> * vec = nullptr;
	int colliderNum = 0;
};

class r3dRaycastCallback : public reactphysics3d::RaycastCallback {
public:
	r3dRaycastCallback(std::map< reactphysics3d::CollisionBody *, Collider_v2*> * _map, bool & _isCollided) :
		mBody_Collider_MAP(_map),
		mIsCollided(_isCollided) 
	{
		mCheckOnce = true;
	}

	r3dRaycastCallback(std::map< reactphysics3d::CollisionBody *, Collider_v2*> * _map, std::list<RaycastResult> * _rayresult, bool & _isCollided) :
		mBody_Collider_MAP(_map),
		mIsCollided(_isCollided),
		mResultList(_rayresult) 
	{
		mCheckOnce = false;
	}

	reactphysics3d::decimal notifyRaycastHit(const reactphysics3d::RaycastInfo& raycastInfo) override {
		auto iter = mBody_Collider_MAP->find(raycastInfo.body);

		if (iter != mBody_Collider_MAP->end()) {
			mIsCollided = true;

			result.mHitFraction = raycastInfo.hitFraction;
			result.mWorldNormal = Rvec3ToDfloat3(raycastInfo.worldNormal);
			result.mWorldPoint = Rvec3ToDfloat3(raycastInfo.worldPoint);
			result.mCollider = iter->second;

			if (mResultList != nullptr) mResultList->push_back(result);
		}

		return mCheckOnce ? 0.0f : 1.0f;
	}

	RaycastResult getResult() {
		return result;
	}

	std::map< reactphysics3d::CollisionBody *, Collider_v2*> *mBody_Collider_MAP;
	std::list<RaycastResult> * mResultList = nullptr;
	RaycastResult result;
	bool mCheckOnce, &mIsCollided;
};



PhysicsModule::~PhysicsModule()
{
	delete m_World;
	delete m_Gravity;
}

bool PhysicsModule::Initialize()
{
	m_Gravity = new reactphysics3d::Vector3(0.0, -9.81, 0.0);
	m_World = new reactphysics3d::DynamicsWorld(*m_Gravity);
	m_World->setNbIterationsVelocitySolver(15);
	m_World->setNbIterationsPositionSolver(8);

	for (auto it = m_Colliders.begin(); it != m_Colliders.end(); it++) {
		reactphysics3d::CollisionBody* collisionBody = (*it)->initialize_React3D(m_World);
		m_ColliderMap.insert(std::make_pair(collisionBody, (*it).get()));
	}

	return true;
}

void PhysicsModule::PhysicsCompoInit(std::shared_ptr<Collider_v2> _component)
{
	reactphysics3d::CollisionBody* collisionBody = _component->initialize_React3D(m_World);
	m_ColliderMap.insert(std::make_pair(collisionBody, _component.get()));
}

void PhysicsModule::Update()
{
	UpdateComponent();
}

void PhysicsModule::PreUpdate()
{
	for (auto it = m_Colliders.begin(); it != m_Colliders.end(); it++) {
		if ((*it)->GameObject.enabled &&
			(*it)->Enabled)
		{
			(*it)->BeforePhysicsUpdate();
		}
	}
}

void PhysicsModule::PhysicsUpdate()
{
	PreUpdate();
	m_World->update(Timer::GetDeltaTime());
	UpdateComponent();
	CollisionTest_ver2();

	//UpdateCollider();
	//CollisionTest();
	//react3DCollisioinTest();
}

void PhysicsModule::UpdateCollider()
{
	/*for (std::vector<std::shared_ptr<Collider>>::iterator it = colliderBuffer->begin(); it != colliderBuffer->end(); it++) {
		if ((*it) == nullptr) assert("collider buffer have nullptr" && 1 == 0);

		bool gameObjectValid = (*it)->gameObject->enabled;
		bool componentValid = (*it)->enabled;
		if (gameObjectValid && componentValid == false) continue;

		(*it)->Update();
	}*/
}

void PhysicsModule::UpdateComponent()
{
	for (auto it = m_Colliders.begin(); it != m_Colliders.end(); it++) {
		if ((*it)->GameObject.enabled &&
			(*it)->Enabled) 
		{
			(*it)->Update();
		}
	}
}

void PhysicsModule::CollisionTest()
{
	/*typedef std::vector<std::shared_ptr<Collider>>::iterator ITERATOR;

	ITERATOR begin = colliderBuffer->begin();
	ITERATOR end = colliderBuffer->end();
	int test_Range = 1;

	//콜리더 갯수 n 에서 n*(n-1) / 2 번 계산
	for (ITERATOR source = begin + 1; source != end; source++) {
		bool gameObjectValid = (*source)->gameObject->enabled;
		bool componentValid = (*source)->enabled;
		if (gameObjectValid && componentValid == false) continue;

		for (ITERATOR target = begin; target != begin + test_Range; target++) {
			gameObjectValid = (*target)->gameObject->enabled;
			componentValid = (*target)->enabled;
			if (gameObjectValid && componentValid == false) continue;

			//DirectX::ContainmentType result = (*source)->Debug_CollisionTest((*target));
			MyCustom::CollisionType result = (*source)->My_CollisionTest(target->get());

			if (result == 0) continue;

			(*source)->collidObj.push_back(target->get());
			(*target)->collidObj.push_back(source->get());
		}

		test_Range++;
	}*/
}

void PhysicsModule::CollisionTest_ver2()
{
	//콜리더 갯수 n 에서 n*(n-1) / 2 번 계산

	for (auto& ptr : m_Colliders) {
		if (ptr->GameObject.enabled &&
			ptr->Enabled &&
			ptr->colllisionTestOn == false)
		{
			continue;
		}
		react3DCollisioinTest(ptr.get());
	}
}

void PhysicsModule::react3DCollisioinTest()
{
	r3dCallback testCall;

	m_World->testCollision(&testCall);
	//mReactPhysics_DYNAMIC_WORLD->testCollision()
}

void PhysicsModule::react3DCollisioinTest(Collider_v2* _collider)
{
	r3dCollisionCallback collisionCallback(_collider, &m_ColliderMap);

	reactphysics3d::CollisionBody * body = _collider->mRigidBody;

	m_World->testCollision(body, &collisionCallback);
}

void PhysicsModule::RegisterComponent(const std::shared_ptr<Collider_v2>& compo)
{
	m_Colliders.push_back(compo);
}

void PhysicsModule::DeregisterComponent(const std::shared_ptr<Collider_v2>& compo)
{
	auto iter = std::find(m_Colliders.begin(), m_Colliders.end(), compo);

	if (iter != m_Colliders.end()) {
		m_Colliders.erase(iter);
	}
}

bool PhysicsModule::Raycast(DirectX::XMFLOAT3 & from, DirectX::XMFLOAT3 & to)
{
	bool isCollided;

	reactphysics3d::Vector3 start(from.x, from.y, from.z), end(to.x, to.y, to.z);
	reactphysics3d::Ray ray(start, end);

	r3dRaycastCallback callBack(&m_ColliderMap, isCollided);
	m_World->raycast(ray, &callBack);

	return isCollided;
}

bool PhysicsModule::Raycast(DirectX::XMFLOAT3 & from, DirectX::XMFLOAT3 & to, RaycastResult & rayInfo)
{
	bool isCollided;

	reactphysics3d::Vector3 start(from.x, from.y, from.z), end(to.x, to.y, to.z);
	reactphysics3d::Ray ray(start, end);

	r3dRaycastCallback callBack(&m_ColliderMap, isCollided);
	m_World->raycast(ray, &callBack);
	rayInfo = callBack.getResult();

	return isCollided;
}

bool PhysicsModule::RaycastAll(DirectX::XMFLOAT3 & from, DirectX::XMFLOAT3 & to, std::list<RaycastResult>& rayInfo)
{
	bool isCollided;

	reactphysics3d::Vector3 start(from.x, from.y, from.z), end(to.x, to.y, to.z);
	reactphysics3d::Ray ray(start, end);

	r3dRaycastCallback callBack(&m_ColliderMap, &rayInfo, isCollided);
	m_World->raycast(ray, &callBack);

	return isCollided;
}