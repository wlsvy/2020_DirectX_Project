#include "GameObject.h"

using DirectX::operator+=;

void GameObject::UpdateMatrix()
{
	assert("UpdateMatrix ust be override" && 0);
}



const DirectX::XMVECTOR & GameObject::GetPositionVector() const
{
	return this->posVector;
}

const DirectX::XMFLOAT3 & GameObject::GetPositionFloat3() const
{
	return this->pos;
}

const DirectX::XMVECTOR & GameObject::GetRotationVector() const
{
	return this->rotVector;
}

const DirectX::XMFLOAT3 & GameObject::GetRotationFloat3() const
{
	return this->rot;
}

void GameObject::SetPosition(const DirectX::XMVECTOR & pos)
{
	DirectX::XMStoreFloat3(&this->pos, pos);
	this->posVector = pos;
	this->UpdateMatrix();
}

void GameObject::SetPosition(const DirectX::XMFLOAT3 & pos)
{

	this->pos = pos;
	this->posVector = DirectX::XMLoadFloat3(&this->pos);
	this->UpdateMatrix();
}

void GameObject::SetPosition(float x, float y, float z)
{
	this->pos = DirectX::XMFLOAT3(x, y, z);
	this->posVector = DirectX::XMLoadFloat3(&this->pos);
	this->UpdateMatrix();
}

void GameObject::AdjustPosition(const DirectX::XMVECTOR & pos)
{
	this->posVector += pos;
	DirectX::XMStoreFloat3(&this->pos, this->posVector);
	this->UpdateMatrix();
}

void GameObject::AdjustPosition(float x, float y, float z)
{
	this->pos.x += x;
	this->pos.y += y;
	this->pos.z += z;
	this->posVector = DirectX::XMLoadFloat3(&this->pos);
	this->UpdateMatrix();
}

void GameObject::SetRotation(const DirectX::XMVECTOR & rot)
{
	this->rotVector = rot;
	DirectX::XMStoreFloat3(&this->rot, rot);
	this->UpdateMatrix();
}

void GameObject::SetRotation(const DirectX::XMFLOAT3 & rot)
{
	this->rot = rot;
	this->rotVector = DirectX::XMLoadFloat3(&this->rot);
	this->UpdateMatrix();
}

void GameObject::SetRotation(float x, float y, float z)
{
	this->rot = DirectX::XMFLOAT3(x, y, z);
	this->rotVector = DirectX::XMLoadFloat3(&this->rot);
	this->UpdateMatrix();
}

void GameObject::AdjustRotation(const DirectX::XMVECTOR & rot)
{
	this->rotVector += rot;
	DirectX::XMStoreFloat3(&this->rot, this->rotVector);
	this->UpdateMatrix();
}

void GameObject::AdjustRotation(const DirectX::XMFLOAT3 & rot)
{
	this->rot.x += rot.x;
	this->rot.y += rot.y;
	this->rot.z += rot.z;
	this->rotVector = DirectX::XMLoadFloat3(&this->rot);
	this->UpdateMatrix();
}

void GameObject::AdjustRotation(float x, float y, float z)
{
	this->rot.x += x;
	this->rot.y += y;
	this->rot.z += z;
	this->rotVector = DirectX::XMLoadFloat3(&this->rot);
	this->UpdateMatrix();
}

void GameObject::SetScale(float xScale, float yScale, float zScale)
{
	scale.x = xScale;
	scale.y = yScale;
	scale.z = zScale;
	UpdateMatrix();
}
