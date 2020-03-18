#pragma once
#include <vector>
#include <list>
#include <DirectXMath.h>

class GameObject_v2;
class Collider_v2;

class CollisionInfo {
public:
	GameObject_v2 * mGameObject;
	Collider_v2 * mCollider;
	std::list<DirectX::XMFLOAT3> mContactPoints;
	std::list<DirectX::XMVECTOR> mCenterContactVec;
};