#pragma once
#include <vector>
#include <memory>
#include <DirectXMath.h>
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

		auto model = Pool::CreateInstance<Model>();
		model->Initialize(
			std::vector<Mesh>(1, 
				Mesh(
					vertices,
					indices,
					std::vector<Texture>(1, *Pool::Find<Texture>("WhiteTexture")),
					DirectX::XMMatrixIdentity())
				)
		);
		model->Name = "Box";
	}

	static void CreateSphere(int slice, int stack) {
		float radius = 0.5f;

		std::vector<Vertex3D> vertices; vertices.reserve(2 + (stack - 1) * (slice + 1));
		std::vector<DWORD> indices; indices.reserve(slice * 6 + (stack - 2) * slice * 6);

		Vertex3D topVertex(0.0f, +radius, 0.0f, 0.0f, 0.0f, 0.0f, +1.0f, 0.0f);
		Vertex3D bottomVertex(0.0f, -radius, 0.0f, 0.0f, 1.0f, 0.0f, -1.0f, 0.0f);


		vertices.push_back(topVertex);

		float phiStep = DirectX::XM_PI / stack;
		float thetaStep = 2.0f*XM_PI / slice;

		for (UINT i = 1; i <= stack - 1; ++i)
		{
			float phi = i * phiStep;

			for (UINT j = 0; j <= slice; ++j)
			{
				float theta = j * thetaStep;

				Vertex3D v;

				v.pos.x = radius * sinf(phi)*cosf(theta);
				v.pos.y = radius * cosf(phi);
				v.pos.z = radius * sinf(phi)*sinf(theta);

				XMVECTOR p = XMLoadFloat3(&v.pos);
				XMStoreFloat3(&v.normal, XMVector3Normalize(p));

				v.texCoord.x = theta / XM_2PI;
				v.texCoord.y = phi / XM_PI;

				vertices.push_back(v);
			}
		}

		vertices.push_back(bottomVertex);

		for (UINT i = 1; i <= slice; ++i)
		{
			indices.push_back(0);
			indices.push_back(i + 1);
			indices.push_back(i);
		}

		UINT baseIndex = 1;
		UINT ringVertexCount = slice + 1;
		for (UINT i = 0; i < stack - 2; ++i)
		{
			for (UINT j = 0; j < slice; ++j)
			{
				indices.push_back(baseIndex + i * ringVertexCount + j);
				indices.push_back(baseIndex + i * ringVertexCount + j + 1);
				indices.push_back(baseIndex + (i + 1)*ringVertexCount + j);

				indices.push_back(baseIndex + (i + 1)*ringVertexCount + j);
				indices.push_back(baseIndex + i * ringVertexCount + j + 1);
				indices.push_back(baseIndex + (i + 1)*ringVertexCount + j + 1);
			}
		}

		UINT southPoleIndex = (UINT)vertices.size() - 1;

		baseIndex = southPoleIndex - ringVertexCount;

		for (UINT i = 0; i < slice; ++i)
		{
			indices.push_back(southPoleIndex);
			indices.push_back(baseIndex + i);
			indices.push_back(baseIndex + i + 1);
		}

		auto model = Pool::CreateInstance<Model>();
		model->Initialize(
			std::vector<Mesh>(1,
				Mesh(
					vertices,
					indices,
					std::vector<Texture>(1, *Pool::Find<Texture>("WhiteTexture")),
					DirectX::XMMatrixIdentity())
				)
		);
		model->Name = "Sphere";
	}

	static void Initialize() {
		CreateBox();
		CreateSphere(10, 10);
	}
};