#pragma once
#include <string>
#include "GameObject.h"
#include "../Internal/Graphics/Texture.h"
#include "../Internal/Graphics/Shaders.h"
#include "../Internal/Graphics/IndexBuffer.h"
#include "../Internal/Graphics/VertexBuffer.h"
#include "../Internal/Graphics/Vertex.h"

class Sprite : public GameObject {
public:
	Sprite() {}
	bool Initialize(float width, float height, std::string spritePath);
	void Draw(XMMATRIX orthoMatrix); //2d ī�޶� orthogonal(����) ���
	float GetWidth();
	float GetHeight();
private:
	void UpdateMatrix() override;

	XMMATRIX worldMatrix = XMMatrixIdentity();

	std::unique_ptr<Texture> texture;

	IndexBuffer indices;
	VertexBuffer<Vertex2D> vertices;

};