//#include "Collider.h"
//#include <reactphysics3d/reactphysics3d.h>
//
//using DirectX::operator +=;
//using DirectX::operator +;
//
//void AABBCollider::initialize(const COMPONENT_INIT_DESC & desc)
//{
//	type = MyCustom::COLLIDER_AABB;
//	gameObject = desc.mGameObj;
//	transform = desc.mTransform;
//	Time = desc.mTime;
//	//ComponentID = *desc.componentID;
//}
//
//MyCustom::CollisionType AABBCollider::My_CollisionTest(const Collider * collider)
//{
//	return MyCustom::COLLISION_DISJOINT;
//}
//
//DirectX::ContainmentType AABBCollider::Debug_CollisionTest(const Collider * collider)
//{
//	return DirectX::DISJOINT;
//}
//
////DirectX::ContainmentType AABBCollider::Debug_CollisionTest(const Collider * collider)
////{
////	if (CollisionMask & collider->CollisionMask == 0) return DirectX::ContainmentType::DISJOINT;
////
////	
////	return DirectX::ContainmentType();
////}
//
//void AABBCollider::DebugModel_Draw(DirectX::PrimitiveBatch<DirectX::VertexPositionColor>* batch, DirectX::FXMVECTOR color)
//{
//}
//
//COLLIDER_DEBUG_MODEL AABBCollider::Get_DebugModelType() const
//{
//	COLLIDER_DEBUG_MODEL desc;
//	desc.position = &transform->GetPositionFloat3();
//	desc.rotation = &transform->GetRotationFloat3();
//	desc.scale = &transform->GetScaleFloat3();
//
//	desc.typeNum = 0;
//	desc.aabbPtr = &colliderModel;
//	return desc;
//}
//
//
//void BoxCollider::initialize(const COMPONENT_INIT_DESC & desc)
//{
//	type = MyCustom::COLLIDER_BOX;
//	gameObject = desc.mGameObj;
//	transform = desc.mTransform;
//	Time = desc.mTime;
//	//ComponentID = *desc.componentID;
//}
//
//void BoxCollider::Update()
//{
//	collidObj.clear();
//
//	DirectX::XMFLOAT3 _pos = transform->GetPositionFloat3();
//	colliderModel.Center = DirectX::XMFLOAT3(_pos.x + localCenter.x, _pos.y + localCenter.y, _pos.z + localCenter.z);
//
//	DirectX::XMFLOAT3 _rot = transform->GetRotationFloat3();
//	DirectX::XMVECTOR quaternion = DirectX::XMQuaternionRotationRollPitchYaw(_rot.x + localRotation.x, _rot.y + localRotation.y, _rot.z + localRotation.z);
//	DirectX::XMStoreFloat4(&colliderModel.Orientation, quaternion);
//
//	DirectX::XMFLOAT3 _scale = transform->GetScaleFloat3();
//	colliderModel.Extents = DirectX::XMFLOAT3(_scale.x * localScale.x, _scale.y * localScale.y, _scale.z * localScale.z);
//
//	
//}
//
//bool BoxCollider::Intersect_Test(const BoxCollider *_collider)
//{
//#define GET_DOT(x, y) DirectX::XMVectorGetX(DirectX::XMVector3Dot((x), (y)))
//#define GET_ROTATED_AXIS_VEC(axis, quaternion) DirectX::XMVector3Rotate(DirectX::XMLoadFloat3(axis), quaternion)
//
//	DirectX::XMVECTOR Dist = DirectX::XMVectorSubtract(
//		DirectX::XMLoadFloat3(&_collider->colliderModel.Center),
//		DirectX::XMLoadFloat3(&this->colliderModel.Center));
//
//	DirectX::XMVECTOR rot1 = DirectX::XMLoadFloat4(&this->colliderModel.Orientation);
//	DirectX::XMVECTOR rot2 = DirectX::XMLoadFloat4(&_collider->colliderModel.Orientation);
//
//	DirectX::XMFLOAT3 Asize = this->localScale;
//	DirectX::XMFLOAT3 Bsize = _collider->localScale;
//
//	DirectX::XMFLOAT3 BaseAxis = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);
//
//	BaseAxis.x = 1;					//1,0,0
//	DirectX::XMVECTOR Ax = GET_ROTATED_AXIS_VEC(&BaseAxis, rot1);
//	DirectX::XMVECTOR Bx = GET_ROTATED_AXIS_VEC(&BaseAxis, rot2);
//
//	BaseAxis.x = 0; BaseAxis.y = 1; //0,1,0
//	DirectX::XMVECTOR Ay = GET_ROTATED_AXIS_VEC(&BaseAxis, rot1);
//	DirectX::XMVECTOR By = GET_ROTATED_AXIS_VEC(&BaseAxis, rot2);
//
//	BaseAxis.y = 0; BaseAxis.z = 1; //0,0,1
//	DirectX::XMVECTOR Az = GET_ROTATED_AXIS_VEC(&BaseAxis, rot1);
//	DirectX::XMVECTOR Bz = GET_ROTATED_AXIS_VEC(&BaseAxis, rot2);
//
//	float r0, r1, R;
//	float AxisDot[3][3];
//
//	//1
//	AxisDot[0][0] = fabsf(GET_DOT(Ax, Bx));
//	AxisDot[0][1] = fabsf(GET_DOT(Ax, By));
//	AxisDot[0][2] = fabsf(GET_DOT(Ax, Bz));
//	r0 = Asize.x;
//	r1 = Bsize.x * AxisDot[0][0] + Bsize.y * AxisDot[0][1] + Bsize.z * AxisDot[0][2];
//	R = fabsf(GET_DOT(Dist, Ax));
//	if (R > r0 + r1) return false;
//
//	//2
//	AxisDot[1][0] = fabsf(GET_DOT(Ay, Bx));
//	AxisDot[1][1] = fabsf(GET_DOT(Ay, By));
//	AxisDot[1][2] = fabsf(GET_DOT(Ay, Bz));
//	r0 = Asize.y;
//	r1 = Bsize.x * AxisDot[1][0] + Bsize.y * AxisDot[1][1] + Bsize.z * AxisDot[1][2];
//	R = fabsf(GET_DOT(Dist, Ay));
//	if (R > r0 + r1) return false;
//
//	//3
//	AxisDot[2][0] = fabsf(GET_DOT(Az, Bx));
//	AxisDot[2][1] = fabsf(GET_DOT(Az, By));
//	AxisDot[2][2] = fabsf(GET_DOT(Az, Bz));
//	r0 = Asize.z;
//	r1 = Bsize.x * AxisDot[2][0] + Bsize.y * AxisDot[2][1] + Bsize.z * AxisDot[2][2];
//	R = fabsf(GET_DOT(Dist, Az));
//	if (R > r0 + r1) return false;
//
//	//4
//	r0 = Asize.x * AxisDot[0][0] + Asize.y * AxisDot[0][1] + Asize.z * AxisDot[0][2];
//	r1 = Bsize.x;
//	R = fabsf(GET_DOT(Dist, Bx));
//	if (R > r0 + r1) return false;
//
//	//5
//	r0 = Asize.x * AxisDot[1][0] + Asize.y * AxisDot[1][1] + Asize.z * AxisDot[1][2];
//	r1 = Bsize.y;
//	R = fabsf(GET_DOT(Dist, By));
//	if (R > r0 + r1) return false;
//
//	//6
//	r0 = Asize.x * AxisDot[2][0] + Asize.y * AxisDot[2][1] + Asize.z * AxisDot[2][2];
//	r1 = Bsize.z;
//	R = fabsf(GET_DOT(Dist, Bz));
//	if (R > r0 + r1) return false;
//
//	//7
//	r0 = Asize.y * AxisDot[2][0] + Asize.z * AxisDot[1][0];
//	r1 = Bsize.y * AxisDot[0][2] + Bsize.z * AxisDot[0][1];
//	R = fabsf(GET_DOT(Dist, Az) * AxisDot[1][0] - GET_DOT(Dist, Ay) * AxisDot[2][0]);
//	if (R > r0 + r1) return false;
//
//	//8
//	r0 = Asize.y * AxisDot[2][1] + Asize.z * AxisDot[1][1];
//	r1 = Bsize.x * AxisDot[0][2] + Bsize.z * AxisDot[0][0];
//	R = fabsf(AxisDot[1][1] * GET_DOT(Az, Dist) - AxisDot[2][1] * GET_DOT(Ay, Dist));
//	if (R > r0 + r1) return false;
//
//	//9
//	r0 = Asize.y * AxisDot[2][2] + Asize.z * AxisDot[1][2];
//	r1 = Bsize.x * AxisDot[0][1] + Bsize.y * AxisDot[0][0];
//	R = fabsf(AxisDot[1][2] * GET_DOT(Az, Dist) - AxisDot[2][2] * GET_DOT(Ay, Dist));
//	if (R > r0 + r1) return false;
//
//	//10
//	r0 = Asize.x * AxisDot[2][0] + Asize.z * AxisDot[0][0];
//	r1 = Bsize.y * AxisDot[1][2] + Bsize.z * AxisDot[1][1];
//	R = fabsf(AxisDot[2][0] * GET_DOT(Ax, Dist) - AxisDot[0][0] * GET_DOT(Az, Dist));
//	if (R > r0 + r1) return false;
//
//	//11
//	r0 = Asize.x * AxisDot[2][1] + Asize.z * AxisDot[0][1];
//	r1 = Bsize.x * AxisDot[1][2] + Bsize.z * AxisDot[1][0];
//	R = fabsf(AxisDot[2][1] * GET_DOT(Ax, Dist) - AxisDot[0][1] * GET_DOT(Az, Dist));
//	if (R > r0 + r1) return false;
//
//	//12
//	r0 = Asize.x * AxisDot[2][2] + Asize.z * AxisDot[0][2];
//	r1 = Bsize.x * AxisDot[1][1] + Bsize.y * AxisDot[1][0];
//	R = fabsf(AxisDot[2][2] * GET_DOT(Ax, Dist) - AxisDot[0][2] * GET_DOT(Az, Dist));
//	if (R > r0 + r1) return false;
//
//	//13
//	r0 = Asize.x * AxisDot[1][0] + Asize.y * AxisDot[0][0];
//	r1 = Bsize.y * AxisDot[2][2] + Bsize.z * AxisDot[2][1];
//	R = fabsf(AxisDot[0][0] * GET_DOT(Ay, Dist) - AxisDot[1][0] * GET_DOT(Ax, Dist));
//	if (R > r0 + r1) return false;
//
//	//14
//	r0 = Asize.x * AxisDot[1][1] + Asize.y * AxisDot[0][1];
//	r1 = Bsize.x * AxisDot[2][2] + Bsize.z * AxisDot[2][0];
//	R = fabsf(AxisDot[0][1] * GET_DOT(Ay, Dist) - AxisDot[1][1] * GET_DOT(Ax, Dist));
//	if (R > r0 + r1) return false;
//
//	//15
//	r0 = Asize.x * AxisDot[1][2] + Asize.y * AxisDot[0][2];
//	r1 = Bsize.x * AxisDot[2][1] + Bsize.y * AxisDot[2][0];
//	R = fabsf(AxisDot[0][2] * GET_DOT(Ay, Dist) - AxisDot[1][2] * GET_DOT(Ax, Dist));
//	if (R > r0 + r1) return false;
//
//
//#undef GET_DOT
//#undef GET_ROTATED_AXIS_VEC
//	return true;
//}
//
//MyCustom::CollisionType BoxCollider::My_CollisionTest(const Collider * collider)
//{
//	if ((CollisionMask & collider->CollisionMask) == 0) return MyCustom::COLLISION_DISJOINT;
//	if (collider->type != MyCustom::COLLIDER_BOX) assert("Custom Collider is not established" && 1 == 0);
//	
//	COLLIDER_DEBUG_MODEL desc = collider->Get_DebugModelType();
//	bool result = Intersect_Test(desc.boxCollider);
//
//	return result ? MyCustom::COLLISION_INTERSECT : MyCustom::COLLISION_DISJOINT;
//}
//
//DirectX::ContainmentType BoxCollider::Debug_CollisionTest(const Collider * collider)
//{
//	if((CollisionMask & collider->CollisionMask) == 0) return DirectX::DISJOINT;
//
//	switch (collider->type) {
//	case MyCustom::COLLIDER_BOX :
//		COLLIDER_DEBUG_MODEL desc = collider->Get_DebugModelType();
//		return colliderModel.Contains(*desc.boxPtr);
//		break;
//	}
//	
//	return DirectX::DISJOINT;
//}
//
//void BoxCollider::DebugModel_Draw(DirectX::PrimitiveBatch<DirectX::VertexPositionColor>* batch, DirectX::FXMVECTOR color)
//{
//}
//
//COLLIDER_DEBUG_MODEL BoxCollider::Get_DebugModelType() const
//{
//	COLLIDER_DEBUG_MODEL desc;
//	desc.position = &transform->GetPositionFloat3();
//	desc.rotation = &transform->GetRotationFloat3();
//	desc.scale = &transform->GetScaleFloat3();
//	
//	desc.typeNum = 1;
//	desc.boxPtr = &colliderModel;
//	desc.boxCollider = this;
//	return desc;
//}
//
//
//
//void SphereCollider::initialize(const COMPONENT_INIT_DESC & desc)
//{
//	type = MyCustom::COLLIDER_SPHERE;
//	gameObject = desc.mGameObj;
//	transform = desc.mTransform;
//	Time = desc.mTime;
//	//ComponentID = *desc.componentID;
//}
//
//MyCustom::CollisionType SphereCollider::My_CollisionTest(const Collider * collider)
//{
//	return MyCustom::COLLISION_DISJOINT;
//}
//
//DirectX::ContainmentType SphereCollider::Debug_CollisionTest(const Collider * collider)
//{
//	return DirectX::DISJOINT;
//}
//
//void SphereCollider::DebugModel_Draw(DirectX::PrimitiveBatch<DirectX::VertexPositionColor>* batch, DirectX::FXMVECTOR color)
//{
//}
//
//COLLIDER_DEBUG_MODEL SphereCollider::Get_DebugModelType() const
//{
//	COLLIDER_DEBUG_MODEL desc;
//	desc.position = &transform->GetPositionFloat3();
//	desc.rotation = &transform->GetRotationFloat3();
//	desc.scale = &transform->GetScaleFloat3();
//
//	desc.typeNum = 2;
//	desc.spherePtr = &colliderModel;
//	return desc;
//}
