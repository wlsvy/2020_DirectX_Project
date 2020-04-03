#include "RenderableGameObject.h"

#include "../Internal/Core/InternalHelper.h"
#include "../Component/Transform.h"

bool RenderableGameObject::Initialize(const std::string & filePath)
{
	if (!model.Initialize(filePath)) {
		ErrorLogger::Log("test model Initialize Failed");
		return false;
	}

	return true;
}

void RenderableGameObject::Draw(const DirectX::XMMATRIX & viewProjectionMatrix)
{
	model.Draw(m_Transform->GetWorldMatrix(), viewProjectionMatrix);
}