#pragma once
#include <vector>
#include <memory>
#include <DirectXMath.h>
#include <DirectXCollision.h>
#include "Mesh.h"
#include "Model.h"
#include "Texture.h"
#include "../Core/ObjectPool.h"

namespace BaseGeometry {
	using DirectX::operator+;
	using DirectX::operator-;
	using DirectX::operator/=;

	static void CreateBox() {
		const std::vector<Vertex3D> vertices = {//position	//texcoord		//normal
			{-0.5f, -0.5f, -0.5f,		0.0f, 1.0f,		 0.0f, 0.0f, -1.0f},
			{-0.5f, +0.5f, -0.5f,		0.0f, 0.0f,		 0.0f, 0.0f, -1.0f},
			{+0.5f, +0.5f, -0.5f,		1.0f, 0.0f,		 0.0f, 0.0f, -1.0f},
			{+0.5f, -0.5f, -0.5f,		1.0f, 1.0f,		 0.0f, 0.0f, -1.0f},
			{-0.5f, -0.5f, +0.5f,		1.0f, 1.0f,		 0.0f, 0.0f, 1.0f},
			{+0.5f, -0.5f, +0.5f,		0.0f, 1.0f,		 0.0f, 0.0f, 1.0f},
			{+0.5f, +0.5f, +0.5f,		0.0f, 0.0f,		 0.0f, 0.0f, 1.0f},
			{-0.5f, +0.5f, +0.5f,		1.0f, 0.0f,		 0.0f, 0.0f, 1.0f},
			{-0.5f, +0.5f, -0.5f,		0.0f, 1.0f,		  0.0f, 1.0f, 0.0f},
			{-0.5f, +0.5f, +0.5f,		0.0f, 0.0f,		  0.0f, 1.0f, 0.0f},
			{+0.5f, +0.5f, +0.5f,		1.0f, 0.0f,		  0.0f, 1.0f, 0.0f},
			{+0.5f, +0.5f, -0.5f,		1.0f, 1.0f,		  0.0f, 1.0f, 0.0f},
			{-0.5f, -0.5f, -0.5f,		1.0f, 1.0f,		  0.0f, -1.0f, 0.0f},
			{+0.5f, -0.5f, -0.5f,		0.0f, 1.0f,		  0.0f, -1.0f, 0.0f},
			{+0.5f, -0.5f, +0.5f,		0.0f, 0.0f,		  0.0f, -1.0f, 0.0f},
			{-0.5f, -0.5f, +0.5f,		1.0f, 0.0f,		  0.0f, -1.0f, 0.0f},
			{-0.5f, -0.5f, +0.5f,		0.0f, 1.0f,		  -1.0f, 0.0f, 0.0f},
			{-0.5f, +0.5f, +0.5f,		0.0f, 0.0f,		  -1.0f, 0.0f, 0.0f},
			{-0.5f, +0.5f, -0.5f,		1.0f, 0.0f,		  -1.0f, 0.0f, 0.0f},
			{-0.5f, -0.5f, -0.5f,		1.0f, 1.0f,		  -1.0f, 0.0f, 0.0f},
			{+0.5f, -0.5f, -0.5f,		0.0f, 1.0f,		  1.0f, 0.0f, 0.0f},
			{+0.5f, +0.5f, -0.5f,		0.0f, 0.0f,		  1.0f, 0.0f, 0.0f},
			{+0.5f, +0.5f, +0.5f,		1.0f, 0.0f,		  1.0f, 0.0f, 0.0f},
			{+0.5f, -0.5f, +0.5f,		1.0f, 1.0f,		  1.0f, 0.0f, 0.0f}
		};
		const std::vector<DWORD> indices = {
			0,	1,	2,
			0,	2,	3,
			4,	5,	6,
			4,	6,	7,
			8,	9,	10,
			8,	10,	11,
			12,	13,	14,
			12,	14,	15,
			16,	17,	18,
			16,	18,	19,
			20,	21,	22,
			20,	22,	23
		};

		auto model = Pool::CreateInstance<Model>();
		model->Initialize(
			std::vector<std::shared_ptr<MeshBase>>(1, Pool::CreateInstance<Mesh>(
				vertices,
				indices,
				std::vector<Texture>(1, *Pool::Find<Texture>("WhiteTexture")),
				DirectX::XMMatrixIdentity(),
				"Box"
				)));
		model->Name = "Box";
	}
	static void CreatePlane() {
		const std::vector<Vertex3D> vertices = {//position	//texcoord		//normal
			{-0.5, -0.5, 0.0f,     0.0f, 1.0f,    0.0f, 0.0f, -1.0f},
			{-0.5, +0.5, 0.0f,     0.0f, 0.0f,    0.0f, 0.0f, -1.0f},
			{+0.5, +0.5, 0.0f,     1.0f, 0.0f,    0.0f, 0.0f, -1.0f},
			{+0.5, -0.5, 0.0f,     1.0f, 1.0f,    0.0f, 0.0f, -1.0f}
		};
		const std::vector<DWORD> indices = {
			0, 1, 2,
			0, 2, 3,
		};

		auto model = Pool::CreateInstance<Model>();
		model->Initialize(
			std::vector<std::shared_ptr<MeshBase>>(1, Pool::CreateInstance<Mesh>(
				vertices,
				indices,
				std::vector<Texture>(1, *Pool::Find<Texture>("WhiteTexture")),
				DirectX::XMMatrixIdentity(),
				"Plane"
				)));
		model->Name = "Plane";
	}
	static void CreateSphere(UINT slice, UINT stack) {
		float radius = 0.5f;

		std::vector<Vertex3D> vertices; 
		vertices.reserve(2 + (stack - 1) * (slice + 1));
		std::vector<DWORD> indices; 
		indices.reserve(slice * 6 + (stack - 2) * slice * 6);

		Vertex3D topVertex(0.0f, +radius, 0.0f, 0.0f, 0.0f, 0.0f, +1.0f, 0.0f);
		Vertex3D bottomVertex(0.0f, -radius, 0.0f, 0.0f, 1.0f, 0.0f, -1.0f, 0.0f);


		vertices.push_back(topVertex);

		float phiStep = DirectX::XM_PI / stack;
		float thetaStep = 2.0f * DirectX::XM_PI / slice;

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

				DirectX::XMVECTOR p = DirectX::XMLoadFloat3(&v.pos);
				DirectX::XMStoreFloat3(&v.normal, DirectX::XMVector3Normalize(p));

				v.texCoord.x = theta / DirectX::XM_2PI;
				v.texCoord.y = phi / DirectX::XM_PI;

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
			std::vector<std::shared_ptr<MeshBase>>(1, Pool::CreateInstance<Mesh>(
				vertices,
				indices,
				std::vector<Texture>(1, *Pool::Find<Texture>("WhiteTexture")),
				DirectX::XMMatrixIdentity(),
				"Sphere"
				)));
		model->Name = "Sphere";
	}
	
	static void Initialize() {
		CreateBox();
		CreatePlane();
		CreateSphere(10, 10);
	}
};