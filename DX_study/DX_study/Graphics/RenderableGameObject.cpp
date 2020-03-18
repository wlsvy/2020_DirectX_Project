#include "RenderableGameObject.h"

bool RenderableGameObject::Initialize(Model * _model)
{
	model = _model;


	this->SetPosition(0.0f, 0.0f, 0.0f);
	this->SetRotation(0.0f, 0.0f, 0.0f);
	this->UpdateMatrix();
	return true;
}

void RenderableGameObject::Draw(const DirectX::XMMATRIX & viewProjectionMatrix)
{
	model->Draw(this->worldMatrix, viewProjectionMatrix);
}

void RenderableGameObject::UpdateMatrix()
{
	this->worldMatrix = DirectX::XMMatrixRotationRollPitchYaw(this->rot.x, this->rot.y, this->rot.z) * DirectX::XMMatrixTranslation(this->pos.x, this->pos.y, this->pos.z);
	this->UpdateDirectionVectors();
}