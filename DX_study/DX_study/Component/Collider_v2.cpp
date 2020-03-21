#include "Collider_v2.h"

#include <reactphysics3d/reactphysics3d.h>

#include "GameObject_v2.h"

using DirectX::operator +=;
using DirectX::operator +;

std::string Float2String(const float _val) {
	std::ostringstream oss;
	oss << _val;
	std::string str = oss.str();
	size_t dotpos = str.find_first_of('.');
	size_t ipos = str.size() - 1;

	if (dotpos != std::string::npos) {
		
		while (str[ipos] == '0' && ipos > dotpos) {
			--ipos;
		}
		//str.erase(ipos + 1, std::string::npos);
	}
	return str.substr(0, ipos + 1);
}

#pragma region COLLIDER

Collider_v2::Collider_v2(GameObject_v2 & obj) : Component(obj), transform(obj.transform) {}

MyCustom::COLLIDER_VER2 Collider_v2::getColliderType()
{
	return mColliderType;
}

void Collider_v2::ColliderUpdate()
{
	reactphysics3d::Vector3 pos(transform.position.x, transform.position.y, transform.position.z);
	DirectX::XMFLOAT4 D_quat = transform.GetQuaternionFloat4();
	reactphysics3d::Quaternion quat(D_quat.x, D_quat.y, D_quat.z, D_quat.w);
	reactphysics3d::Transform t_form(pos, quat);
	mRigidBody->setTransform(t_form);
}

void Collider_v2::TransformUpdate(reactphysics3d::Transform & _prevTrasnform)
{
	reactphysics3d::Transform t_form = mRigidBody->getTransform();
	reactphysics3d::Transform lerpedTransform = reactphysics3d::Transform::interpolateTransforms(_prevTrasnform, t_form, 0.2);
	_prevTrasnform = lerpedTransform;

	reactphysics3d::Vector3 pos = lerpedTransform.getPosition();
	reactphysics3d::Quaternion quat = lerpedTransform.getOrientation();
	DirectX::XMVECTOR quatVec = DirectX::XMVectorSet(quat.x, quat.y, quat.z, quat.w);

	transform.SetPosition(pos.x, pos.y, pos.z);
	transform.SetRotation(quatVec);
}

void Collider_v2::BeforePhysicsUpdate()
{
	mCollisionInfo.clear();
	//if (transform.TRANSFORM_UPDATED) ColliderUpdate();
}

#pragma endregion

#pragma region BOX_COLLIDER

BoxCollider_ver2::BoxCollider_ver2(GameObject_v2 & obj) : Collider_v2(obj)
{
	std::strcpy(Name, "BoxCollider");
	colllisionTestOn = true;
}

reactphysics3d::RigidBody* BoxCollider_ver2::initialize_React3D(reactphysics3d::DynamicsWorld * _world)
{
	reactphysics3d::Vector3 pos(transform.position.x, transform.position.y, transform.position.z);
	reactphysics3d::Quaternion quat = reactphysics3d::Quaternion::fromEulerAngles(transform.rotation.x, transform.rotation.y, transform.rotation.z);
	reactphysics3d::Transform t_form(pos, quat);

	mRigidBody = _world->createRigidBody(t_form);
	mRigidBody->setType(reactphysics3d::BodyType::STATIC);

	pos = reactphysics3d::Vector3(localCenter.x, localCenter.y, localCenter.z);
	quat = reactphysics3d::Quaternion::fromEulerAngles(localRotation.x, localRotation.y, localRotation.z);
	t_form = reactphysics3d::Transform(pos, quat);

	DirectX::XMFLOAT3 _scale = transform.GetScaleFloat3();
	mExtent = DirectX::XMFLOAT3(_scale.x * localScale.x, _scale.y * localScale.y, _scale.z * localScale.z);
	reactphysics3d::Vector3 extent(mExtent.x, mExtent.y, mExtent.z);

	mBoxShape = new reactphysics3d::BoxShape(extent);
	mProxyShape = mRigidBody->addCollisionShape(mBoxShape, t_form, 1.0f);

	return mRigidBody;
}

COLLIDER_DEBUG_MODEL_VER2 BoxCollider_ver2::Get_DebugModelType() const
{
	reactphysics3d::Transform t_form = mRigidBody->getTransform();
	reactphysics3d::Vector3 pos = t_form.getPosition();
	reactphysics3d::Quaternion quat = t_form.getOrientation();

	DirectX::XMFLOAT3 Dpos(pos.x, pos.y, pos.z);
	DirectX::XMFLOAT4 Dquat(quat.x, quat.y, quat.z, quat.w);

	COLLIDER_DEBUG_MODEL_VER2 desc;
	desc.typeNum = MyCustom::COLLIDER_VER2::COLLIDER_BOX_VER2;
	desc.mDeubgBox = DirectX::BoundingOrientedBox(Dpos, mExtent, Dquat);
	return desc;
}

BoxCollider_ver2::~BoxCollider_ver2()
{
	delete mBoxShape;
}

void BoxCollider_ver2::OnGui()
{
	std::string text = "Local Center : " + Float2String(localCenter.x) + " " + Float2String(localCenter.y) + " " + Float2String(localCenter.z);
	ImGui::Text(text.c_str());
	text = "Local Scale : " + Float2String(localScale.x) + " " + Float2String(localScale.y) + " " + Float2String(localScale.z);
	ImGui::Text(text.c_str());
	text = "Local Rotation : " + Float2String(localRotation.x) + " " + Float2String(localRotation.y) + " " + Float2String(localRotation.z);
	ImGui::Text(text.c_str());

	const unsigned short u_16 = 1;
	ImGui::InputScalar("Collision Mask", ImGuiDataType_U16, &CollisionMask, &u_16, NULL, "%u");
	if (ImGui::Button("Update Bitmask")) mProxyShape->setCollideWithMaskBits(CollisionMask);
}

#pragma endregion

#pragma region SPHERE_COLLIDER

SphereCollider_ver2::SphereCollider_ver2(GameObject_v2 & obj) : Collider_v2(obj) {
	std::strcpy(Name, "Sphere Collider");
	colllisionTestOn = true;
}

COLLIDER_DEBUG_MODEL_VER2 SphereCollider_ver2::Get_DebugModelType() const
{
	reactphysics3d::Transform t_form = mRigidBody->getTransform();
	reactphysics3d::Vector3 pos = t_form.getPosition();

	DirectX::XMFLOAT3 Dpos(pos.x, pos.y, pos.z);

	COLLIDER_DEBUG_MODEL_VER2 desc;
	desc.typeNum = MyCustom::COLLIDER_VER2::COLLIDER_SPHERE_VER2;
	desc.mDebugSphere = DirectX::BoundingSphere(Dpos, Radius);
	return desc;
}

SphereCollider_ver2::~SphereCollider_ver2()
{
	delete mSphereShape;
}

void SphereCollider_ver2::OnGui()
{
	std::string text = "Local Center : " + Float2String(localCenter.x) + " " + Float2String(localCenter.y) + " " + Float2String(localCenter.z);
	ImGui::Text(text.c_str());
	text = "Radius : " + Float2String(Radius);
	ImGui::Text(text.c_str());

	const unsigned short u_16 = 1;
	ImGui::InputScalar("Collision Mask", ImGuiDataType_U16, &CollisionMask, &u_16, NULL, "%u");
	if (ImGui::Button("Update Bitmask")) mProxyShape->setCollideWithMaskBits(CollisionMask);
}

reactphysics3d::RigidBody* SphereCollider_ver2::initialize_React3D(reactphysics3d::DynamicsWorld * _world)
{
	reactphysics3d::Vector3 pos(transform.position.x, transform.position.y, transform.position.z);
	reactphysics3d::Quaternion quat = reactphysics3d::Quaternion::fromEulerAngles(transform.rotation.x, transform.rotation.y, transform.rotation.z);
	reactphysics3d::Transform t_form(pos, quat);

	mRigidBody = _world->createRigidBody(t_form);
	mRigidBody->setType(reactphysics3d::BodyType::STATIC);

	pos = reactphysics3d::Vector3(localCenter.x, localCenter.y, localCenter.z);
	quat = reactphysics3d::Quaternion::identity();
	t_form = reactphysics3d::Transform(pos, quat);

	DirectX::XMFLOAT3 _scale = transform.GetScaleFloat3();

	mSphereShape = new reactphysics3d::SphereShape(Radius);
	mProxyShape = mRigidBody->addCollisionShape(mSphereShape, t_form, 1.0f);

	return mRigidBody;
}

#pragma endregion

#pragma region BOX_RIGIDBODY

BoxRigidBody::BoxRigidBody(GameObject_v2 & obj) : Collider_v2(obj)
{
	std::strcpy(Name, "Box RigidBody");
}

COLLIDER_DEBUG_MODEL_VER2 BoxRigidBody::Get_DebugModelType() const
{
	reactphysics3d::Transform t_form = mRigidBody->getTransform();
	reactphysics3d::Vector3 pos = t_form.getPosition();
	reactphysics3d::Quaternion quat = t_form.getOrientation();

	DirectX::XMFLOAT3 Dpos(pos.x, pos.y, pos.z);
	DirectX::XMFLOAT4 Dquat(quat.x, quat.y, quat.z, quat.w);

	COLLIDER_DEBUG_MODEL_VER2 desc;
	desc.typeNum = MyCustom::COLLIDER_VER2::COLLIDER_BOX_VER2;
	desc.mDeubgBox = DirectX::BoundingOrientedBox(Dpos, mExtent, Dquat);
	return desc;
}

BoxRigidBody::~BoxRigidBody()
{
	delete mBoxShape;
	delete mPrevTransform;
}

void BoxRigidBody::OnGui()
{
	std::string text = "Local Center : " + Float2String(localCenter.x) + " " + Float2String(localCenter.y) + " " + Float2String(localCenter.z);
	ImGui::Text(text.c_str());
	text = "Local Scale : " + Float2String(localScale.x) + " " + Float2String(localScale.y) + " " + Float2String(localScale.z);
	ImGui::Text(text.c_str());
	text = "Local Rotation : " + Float2String(localRotation.x) + " " + Float2String(localRotation.y) + " " + Float2String(localRotation.z);
	ImGui::Text(text.c_str());

	//충돌 마테리얼 설정

	ImGui::Separator();

	reactphysics3d::Vector3 LinearVelocity = mRigidBody->getLinearVelocity();
	reactphysics3d::Vector3 AngularVelocity = mRigidBody->getAngularVelocity();
	text = "Linear Velocity : " + Float2String(LinearVelocity.x) + " " + Float2String(LinearVelocity.y) + " " + Float2String(LinearVelocity.z);
	ImGui::Text(text.c_str());
	text = "Angular Velocity : " + Float2String(AngularVelocity.x) + " " + Float2String(AngularVelocity.y) + " " + Float2String(AngularVelocity.z);
	ImGui::Text(text.c_str());

	ImGui::Separator();


	const unsigned short u_16 = 1;
	ImGui::InputScalar("Collision Mask", ImGuiDataType_U16, &CollisionMask, &u_16, NULL, "%u");
	if (ImGui::Button("Update Bitmask")) mProxyShape->setCollideWithMaskBits(CollisionMask);
}

reactphysics3d::RigidBody* BoxRigidBody::initialize_React3D(reactphysics3d::DynamicsWorld * _world)
{
	reactphysics3d::Vector3 pos(transform.position.x, transform.position.y, transform.position.z);
	reactphysics3d::Quaternion quat = reactphysics3d::Quaternion::fromEulerAngles(transform.rotation.x, transform.rotation.y, transform.rotation.z);
	reactphysics3d::Transform t_form(pos, quat);

	mRigidBody = _world->createRigidBody(t_form);
	mRigidBody->setType(reactphysics3d::BodyType::DYNAMIC);

	mPrevTransform = new reactphysics3d::Transform(t_form);

	pos = reactphysics3d::Vector3(localCenter.x, localCenter.y, localCenter.z);
	quat = reactphysics3d::Quaternion::fromEulerAngles(localRotation.x, localRotation.y, localRotation.z);
	t_form = reactphysics3d::Transform(pos, quat);

	DirectX::XMFLOAT3 _scale = transform.GetScaleFloat3();
	mExtent = DirectX::XMFLOAT3(_scale.x * localScale.x, _scale.y * localScale.y, _scale.z * localScale.z);
	reactphysics3d::Vector3 extent(mExtent.x, mExtent.y, mExtent.z);

	mBoxShape = new reactphysics3d::BoxShape(extent);
	mProxyShape = mRigidBody->addCollisionShape(mBoxShape, t_form, 1.0f);

	return mRigidBody;
}

void BoxRigidBody::Update()
{
	TransformUpdate(*mPrevTransform);
}

#pragma endregion

#pragma region SPHERE_RIGIDBODY

SphereRigidBody::SphereRigidBody(GameObject_v2 & obj) : Collider_v2(obj)
{
	std::strcpy(Name, "Sphere RigidBody");
}

COLLIDER_DEBUG_MODEL_VER2 SphereRigidBody::Get_DebugModelType() const
{
	reactphysics3d::Transform t_form = mRigidBody->getTransform();
	reactphysics3d::Vector3 pos = t_form.getPosition();

	DirectX::XMFLOAT3 Dpos(pos.x, pos.y, pos.z);

	COLLIDER_DEBUG_MODEL_VER2 desc;
	desc.typeNum = MyCustom::COLLIDER_VER2::COLLIDER_SPHERE_VER2;
	desc.mDebugSphere = DirectX::BoundingSphere(Dpos, Radius);
	return desc;
}

SphereRigidBody::~SphereRigidBody()
{
	delete mSphereShape;
	delete mPrevTransform;
}

void SphereRigidBody::OnGui()
{
	std::string text = "Local Center : " + Float2String(localCenter.x) + " " + Float2String(localCenter.y) + " " + Float2String(localCenter.z);
	ImGui::Text(text.c_str());
	text = "Radius : " + Float2String(Radius);
	ImGui::Text(text.c_str());

	ImGui::Separator();

	reactphysics3d::Vector3 LinearVelocity = mRigidBody->getLinearVelocity();
	reactphysics3d::Vector3 AngularVelocity = mRigidBody->getAngularVelocity();
	text = "Linear Velocity : " + Float2String(LinearVelocity.x) + " " + Float2String(LinearVelocity.y) + " " + Float2String(LinearVelocity.z);
	ImGui::Text(text.c_str());
	text = "Angular Velocity : " + Float2String(AngularVelocity.x) + " " + Float2String(AngularVelocity.y) + " " + Float2String(AngularVelocity.z);
	ImGui::Text(text.c_str());

	ImGui::Separator();

	const unsigned short u_16 = 1;
	ImGui::InputScalar("Collision Mask", ImGuiDataType_U16, &CollisionMask, &u_16, NULL, "%u");
	if (ImGui::Button("Update Bitmask")) mProxyShape->setCollideWithMaskBits(CollisionMask);
}

reactphysics3d::RigidBody* SphereRigidBody::initialize_React3D(reactphysics3d::DynamicsWorld * _world)
{
	reactphysics3d::Vector3 pos(transform.position.x, transform.position.y, transform.position.z);
	reactphysics3d::Quaternion quat = reactphysics3d::Quaternion::fromEulerAngles(transform.rotation.x, transform.rotation.y, transform.rotation.z);
	reactphysics3d::Transform t_form(pos, quat);

	mRigidBody = _world->createRigidBody(t_form);
	mRigidBody->setType(reactphysics3d::BodyType::DYNAMIC);

	mPrevTransform = new reactphysics3d::Transform(t_form);

	pos = reactphysics3d::Vector3(localCenter.x, localCenter.y, localCenter.z);
	quat = reactphysics3d::Quaternion::identity();
	t_form = reactphysics3d::Transform(pos, quat);

	DirectX::XMFLOAT3 _scale = transform.GetScaleFloat3();

	mSphereShape = new reactphysics3d::SphereShape(Radius);
	mProxyShape = mRigidBody->addCollisionShape(mSphereShape, t_form, 1.0f);

	return mRigidBody;
}

void SphereRigidBody::Update()
{
	TransformUpdate(*mPrevTransform);
}

#pragma endregion

#pragma region CHRACTER_CONTROLLER

CharacterController::CharacterController(GameObject_v2 & obj) : Collider_v2(obj)
{
	std::strcpy(Name, "Character Controller");
	colllisionTestOn = true;
}

COLLIDER_DEBUG_MODEL_VER2 CharacterController::Get_DebugModelType() const
{
	reactphysics3d::Transform t_form = mRigidBody->getTransform();
	reactphysics3d::Vector3 pos = t_form.getPosition();
	reactphysics3d::Quaternion quat = t_form.getOrientation();
	reactphysics3d::Quaternion upquat = reactphysics3d::Quaternion(0.0f, 1.0f, 0.0f, 0.0f);

	upquat = quat * upquat * quat.getConjugate();
	reactphysics3d::Vector3 upVec = upquat.getVectorV();
	DirectX::XMVECTOR upDxVec = DirectX::XMVectorSet(upVec.x, upVec.y, upVec.z, 0.0f);
	DirectX::XMVector3Normalize(upDxVec);

	float HeightVal = Height / 2;
	DirectX::XMFLOAT3 Dpos(pos.x, pos.y, pos.z);
	DirectX::XMVECTOR DxposVec = DirectX::XMLoadFloat3(&Dpos);
	DirectX::XMVECTOR capsuleSpherePos = DirectX::XMVectorAdd(DxposVec, DirectX::XMVectorMultiply(upDxVec, DirectX::XMVectorSet(HeightVal, HeightVal, HeightVal, 0.0f)));
	DirectX::XMStoreFloat3(&Dpos, capsuleSpherePos);

	COLLIDER_DEBUG_MODEL_VER2 desc;
	desc.typeNum = MyCustom::COLLIDER_VER2::COLLIDER_CAPSULE_VER2;
	desc.mDebugSphere = DirectX::BoundingSphere(Dpos, Radius);

	HeightVal = Height * -1;
	capsuleSpherePos = DirectX::XMVectorAdd(DxposVec, DirectX::XMVectorMultiply(upDxVec, DirectX::XMVectorSet(HeightVal, HeightVal, HeightVal, 0.0f)));
	DirectX::XMStoreFloat3(&Dpos, capsuleSpherePos);
	desc.auxVal = Dpos;

	if (mCollisionInfo.size() != 0) {
		desc.auxVal2 = mCollisionInfo.front()->mContactPoints.front();
	}
	return desc;
}

CharacterController::~CharacterController()
{
	delete mCapsuleShape;
	delete mPrevTransform;
}

void CharacterController::OnGui()
{
	std::string text = "Local Center : " + Float2String(localCenter.x) + " " + Float2String(localCenter.y) + " " + Float2String(localCenter.z);
	ImGui::Text(text.c_str());
	text = "Radius : " + Float2String(Radius);
	ImGui::Text(text.c_str());
	text = "Height : " + Float2String(Height);
	ImGui::Text(text.c_str());

	ImGui::Separator();

	reactphysics3d::Vector3 LinearVelocity = mRigidBody->getLinearVelocity();
	reactphysics3d::Vector3 AngularVelocity = mRigidBody->getAngularVelocity();
	text = "Linear Velocity : " + Float2String(LinearVelocity.x) + " " + Float2String(LinearVelocity.y) + " " + Float2String(LinearVelocity.z);
	ImGui::Text(text.c_str());
	text = "Angular Velocity : " + Float2String(AngularVelocity.x) + " " + Float2String(AngularVelocity.y) + " " + Float2String(AngularVelocity.z);
	ImGui::Text(text.c_str());

	ImGui::Separator();

	const unsigned short u_16 = 1;
	ImGui::InputScalar("Collision Mask", ImGuiDataType_U16, &CollisionMask, &u_16, NULL, "%u");
	if (ImGui::Button("Update Bitmask")) mProxyShape->setCollideWithMaskBits(CollisionMask);
}

void CharacterController::Move(DirectX::XMFLOAT3 & _motion)
{
	reactphysics3d::Vector3 movement(_motion.x, _motion.y, _motion.z);
	reactphysics3d::Transform currentTransform = mRigidBody->getTransform();
	movement += currentTransform.getPosition();
	currentTransform.setPosition(movement);
	
	mRigidBody->setTransform(currentTransform);
}

bool CharacterController::IsGrounded()
{
	for (std::list<std::unique_ptr<CollisionInfo>>::iterator iter = mCollisionInfo.begin(); iter != mCollisionInfo.end(); iter++) {
		std::list<DirectX::XMVECTOR> * contactPoints = &(*iter)->mCenterContactVec;
		for (std::list<DirectX::XMVECTOR>::iterator it2 = contactPoints->begin(); it2 != contactPoints->end(); it2++) {
			if (DirectX::XMVectorGetY(*it2) < -0.5f) {
				return true;
			}
		}
	}
	return false;
}

reactphysics3d::RigidBody* CharacterController::initialize_React3D(reactphysics3d::DynamicsWorld * _world)
{
	reactphysics3d::Vector3 pos(transform.position.x, transform.position.y, transform.position.z);
	reactphysics3d::Quaternion quat = reactphysics3d::Quaternion::fromEulerAngles(transform.rotation.x, transform.rotation.y, transform.rotation.z);
	reactphysics3d::Transform t_form(pos, quat);

	mRigidBody = _world->createRigidBody(t_form);
	mRigidBody->setType(reactphysics3d::BodyType::DYNAMIC);
	reactphysics3d::Material &mat = mRigidBody->getMaterial();
	mat.setBounciness(0);
	mat.setFrictionCoefficient(999.0f);
	mat.setRollingResistance(999.0f);

	//mRigidBody->setMass(1000000000000000000000000000000.0f);
	//mRigidBody->setMass(0.0f);
	//mRigidBody->setAngularVelocity(reactphysics3d::Vector3(0.0f, 0.0f, 0.0f));
	//mRigidBody->setLinearVelocity(reactphysics3d::Vector3(0.0f, 0.0f, 0.0f));

	mPrevTransform = new reactphysics3d::Transform(t_form);

	pos = reactphysics3d::Vector3(localCenter.x, localCenter.y, localCenter.z);
	quat = reactphysics3d::Quaternion::identity();
	t_form = reactphysics3d::Transform(pos, quat);

	DirectX::XMFLOAT3 _scale = transform.GetScaleFloat3();

	mCapsuleShape = new reactphysics3d::CapsuleShape(Radius, Height);
	mProxyShape = mRigidBody->addCollisionShape(mCapsuleShape, t_form, 1.0f);

	return mRigidBody;
}

void CharacterController::Update()
{
	//mRigidBody->setLinearVelocity(reactphysics3d::Vector3(0.0f, 0.0f, 0.0f));
	mRigidBody->setAngularVelocity(reactphysics3d::Vector3(0.0f, 0.0f, 0.0f));
	//TransformUpdate(*mPrevTransform);

	DirectX::XMFLOAT4 quat = transform.GetQuaternionFloat4();

	reactphysics3d::Transform t_form = mRigidBody->getTransform();
	reactphysics3d::Transform lerpedTransform = reactphysics3d::Transform::interpolateTransforms(*mPrevTransform, t_form, 0.2);
	t_form.setOrientation(reactphysics3d::Quaternion(quat.x, quat.y, quat.z, quat.w));
	*mPrevTransform = lerpedTransform;
	mRigidBody->setTransform(t_form);

	reactphysics3d::Vector3 pos = lerpedTransform.getPosition();

	transform.SetPosition(pos.x, pos.y, pos.z);
}

#pragma endregion