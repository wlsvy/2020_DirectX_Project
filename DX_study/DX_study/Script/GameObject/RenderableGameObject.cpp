#include "RenderableGameObject.h"

#include "../Internal/Core/InternalHelper.h"

bool RenderableGameObject::Initialize(const std::string & filePath, ConstantBuffer<CB_VS_vertexshader>& cb_vs_vertexshader)
{
	if (!model.Initialize(filePath, cb_vs_vertexshader)) {
		ErrorLogger::Log("test model Initialize Failed");
		return false;
	}


	this->SetPosition(0.0f, 0.0f, 0.0f);
	this->SetRotation(0.0f, 0.0f, 0.0f);
	this->UpdateMatrix();
	return true;
}

void RenderableGameObject::Draw(const DirectX::XMMATRIX & viewProjectionMatrix)
{
	model.Draw(this->worldMatrix, viewProjectionMatrix);
}

void RenderableGameObject::UpdateMatrix()
{
	this->worldMatrix = DirectX::XMMatrixRotationRollPitchYaw(this->rot.x, this->rot.y, this->rot.z) * DirectX::XMMatrixTranslation(this->pos.x, this->pos.y, this->pos.z);
	this->UpdateDirectionVectors();
}