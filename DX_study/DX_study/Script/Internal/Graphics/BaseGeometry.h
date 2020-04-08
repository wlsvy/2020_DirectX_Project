#pragma once
#include <vector>
#include <memory>
#include "Mesh.h"
#include "Model.h"
#include "Texture.h"
#include "../Core/ObjectPool.h"

namespace BaseGeometry {
	static void CreateBox() {
		const std::vector<Vertex3D> vertices = { //position, texcoord, normal
		{ -1.0f, -1.0f, -1.0f,     0.0f, 1.0f,    0.0f, 0.0f, 0.0f},
		{ -1.0f, +1.0f, -1.0f,     0.0f, 0.0f,    0.0f, 0.0f, 0.0f},
		{ +1.0f, +1.0f, -1.0f,     1.0f, 0.0f,    0.0f, 0.0f, 0.0f},
		{ +1.0f, -1.0f, -1.0f,     1.0f, 1.0f,    0.0f, 0.0f, 0.0f},
		{ -1.0f, -1.0f, +1.0f,     1.0f, 0.0f,    0.0f, 0.0f, 0.0f},
		{ -1.0f, +1.0f, +1.0f,     1.0f, 1.0f,    0.0f, 0.0f, 0.0f},
		{ +1.0f, +1.0f, +1.0f,     0.0f, 1.0f,    0.0f, 0.0f, 0.0f},
		{ +1.0f, -1.0f, +1.0f,     0.0f, 0.0f,    0.0f, 0.0f, 0.0f}
		};
		const std::vector<DWORD> indices = {
			// front face
			0, 1, 2,
			0, 2, 3,

			// back face
			4, 6, 5,
			4, 7, 6,

			// left face
			4, 5, 1,
			4, 1, 0,

			// right face
			3, 2, 6,
			3, 6, 7,

			// top face
			1, 5, 6,
			1, 6, 2,

			// bottom face
			4, 0, 3,
			4, 3, 7
		};
		const std::vector<Texture> textures = std::vector<Texture>(1, *Pool::Find<Texture>("WhiteTexture"));

		auto model = Pool::CreateInstance<Model>();
		model->Initialize(
			std::vector<Mesh>(1, 
				Mesh(
					vertices,
					indices,
					textures,
					DirectX::XMMatrixIdentity())
				)
		);
		model->Name = "Box";
	}

	static void Initialize() {
		CreateBox();
	}
};