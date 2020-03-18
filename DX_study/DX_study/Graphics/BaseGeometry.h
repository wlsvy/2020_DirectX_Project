#pragma once
#include "Model.h"

namespace Study_DX {
	struct Point {
		friend class GraphicsManager;
	private:
		Point() {}

		Vertex3D vertices[1] = {//position	//texcoord		//normal
		{ 1.0f, 1.0f, 1.0f,     0.0f, 0.0f,    0.0f, 0.0f, 0.0f } 
		};

		const std::string name = "Point";
		const UINT vertexSize = 1;
	};


	struct Box {
		friend class GraphicsManager;
	private:
		Box();
		
		Vertex3D vertices[8] = {//position	//texcoord		//normal
			{ -1.0f, -1.0f, -1.0f,     0.0f, 1.0f,    0.0f, 0.0f, 0.0f},
			{ -1.0f, +1.0f, -1.0f,     0.0f, 0.0f,    0.0f, 0.0f, 0.0f},
			{ +1.0f, +1.0f, -1.0f,     1.0f, 0.0f,    0.0f, 0.0f, 0.0f},
			{ +1.0f, -1.0f, -1.0f,     1.0f, 1.0f,    0.0f, 0.0f, 0.0f},
			{ -1.0f, -1.0f, +1.0f,     1.0f, 0.0f,    0.0f, 0.0f, 0.0f},
			{ -1.0f, +1.0f, +1.0f,     1.0f, 1.0f,    0.0f, 0.0f, 0.0f},
			{ +1.0f, +1.0f, +1.0f,     0.0f, 1.0f,    0.0f, 0.0f, 0.0f},
			{ +1.0f, -1.0f, +1.0f,     0.0f, 0.0f,    0.0f, 0.0f, 0.0f}
		};
		DWORD indices[36] = {
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

		const std::string name = "Box(Vertex8)";
		const UINT vertexSize = sizeof(vertices) / sizeof(Vertex3D);
		const UINT indexSize = sizeof(indices) / sizeof(DWORD);
	};

	struct Box_Strange {
		friend class GraphicsManager;
	private:
		Box_Strange();
		Vertex3D vertices[24] = {//position	//texcoord		//normal
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

		DWORD indices[36] = {
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

		const std::string name = "Box(Vertex24)";
		const UINT vertexSize = sizeof(vertices) / sizeof(Vertex3D);
		const UINT indexSize = sizeof(indices) / sizeof(DWORD);
	};

	struct Plane {
		friend class GraphicsManager;
	private:
		Vertex3D vertices[8] = {//position	//texcoord		//normal
			{-0.5f, -0.5f, 0.0f,     0.0f, 1.0f,    0.0f, 0.0f, -1.0f},
			{-0.5f, +0.5f, 0.0f,     0.0f, 0.0f,    0.0f, 0.0f, -1.0f},
			{+0.5f, +0.5f, 0.0f,     1.0f, 0.0f,    0.0f, 0.0f, -1.0f},
			{+0.5f, -0.5f, 0.0f,     1.0f, 1.0f,    0.0f, 0.0f, -1.0f},
			{-0.5f, -0.5f, 0.0f,     0.0f, 1.0f,    0.0f, 0.0f, 1.0f},
			{-0.5f, +0.5f, 0.0f,     0.0f, 0.0f,    0.0f, 0.0f, 1.0f},
			{+0.5f, +0.5f, 0.0f,     1.0f, 0.0f,    0.0f, 0.0f, 1.0f},
			{+0.5f, -0.5f, 0.0f,     1.0f, 1.0f,    0.0f, 0.0f, 1.0f}
		};
		DWORD indices[12] = {
			// ¾Õ¸é
			0, 1, 2,
			0, 2, 3,
			// µÞ¸é
			6, 5, 4,
			7, 6, 4
		};

		const std::string name = "Plane(Two sided)";
		const UINT vertexSize = 8;
		const UINT indexSize = 12;
	};

	struct Sphere {
		friend class GraphicsManager;
	public:
		Sphere(UINT _sliceCount = 5, UINT _stackCount = 5);

		const std::string name = "Sphere";
		std::vector<Vertex3D> vertices;
		std::vector<DWORD> indices;
	};

	struct Cylinder {
		friend class GraphicsManager;
	private:
		Cylinder(float bottomRadius = 0.5f, float topRadius = 0.5f, float height = 1.0f, UINT sliceCount = 5, UINT stackCount = 5);
		void BuildCylinderTopCap(float bottomRadius, float topRadius, float height,UINT sliceCount, UINT stackCount);
		void BuildCylinderBottomCap(float bottomRadius, float topRadius, float height, UINT sliceCount, UINT stackCount);

		const std::string name = "Cylinder";
		std::vector<Vertex3D> vertices;
		std::vector<DWORD> indices;
	};

	void calculateNormal(Vertex3D * _vertexBuffer, const UINT _vertexNum, const DWORD * _indexBuffer, const UINT _indexNum);
}
