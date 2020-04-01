#pragma once
#include "GameObject3D.h"

class RenderableGameObject : public GameObject3D {
public:
	bool Initialize(const std::string & filePath);
	void Draw(const DirectX::XMMATRIX & viewProjectionMatrix);

protected:
	Model model;
	void UpdateMatrix() override;


	DirectX::XMMATRIX worldMatrix = DirectX::XMMatrixIdentity();
};