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
	bool Initialize(ID3D11Device * device, ID3D11DeviceContext * deviceContext, float width, float height, std::string spritePath, ConstantBuffer<CB_VS_vertexshader_2d> & cb_vs_vertexshader_2d);
	void Draw(XMMATRIX orthoMatrix); //2d 카메라 orthogonal(직교) 행렬
	float GetWidth();
	float GetHeight();
private:
	void UpdateMatrix() override;

	ConstantBuffer<CB_VS_vertexshader_2d> * cb_vs_vertexshader_2d = nullptr;
	XMMATRIX worldMatrix = XMMatrixIdentity();

	std::unique_ptr<Texture> texture;
	ID3D11DeviceContext * deviceContext = nullptr;

	IndexBuffer indices;
	VertexBuffer<Vertex2D> vertices;

};