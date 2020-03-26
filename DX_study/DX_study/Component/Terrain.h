#pragma once
#include <stdio.h>
#include <fstream>
#include <DirectXMath.h>
#include <vector>

#include "Component.h"
#include "../Graphics/Vertex.h"

struct TERRAIN_INIT_DESC {
	std::vector<Vertex3D> vertexBuffer;
	std::vector<unsigned long> indexBuffer;
};

class Terrain : public Component {
public:
	using ComponentTag = TerrainComponentTag;
private:

	struct HeightMapType {
		float x, y, z;
		float nx, ny, nz;
		float tu, tv;
	};
	struct VertexType
	{
		DirectX::XMFLOAT3 position;
		DirectX::XMFLOAT2 texture;
		DirectX::XMFLOAT3 normal;
	};
	struct VectorType
	{
		float x, y, z;
	};

public:
	Terrain(const COMPONENT_INIT_DESC & desc);
	~Terrain();
	
	TERRAIN_INIT_DESC TerrainProcess(const char * HeightMapPath);
	void Shutdown();
	void Redner();

	bool LoadHeightMap(const char* filename);
	void NormalizeHeightMap();
	bool CalculateNormals();
	void CalculateTextureCoordinates();
	TERRAIN_INIT_DESC CreateTerrainModel();

	int GetIndexCount();

	Transform * const transform;
	int Texture_Repeat_Count = 8;
	const char * heightFilePath;

private:
	int m_terrainWidth = 0;
	int m_terrainHeight = 0;
	int m_vertexCount = 0;
	int m_indexCount = 0;
	HeightMapType * m_heightMap = nullptr;

	Renderer * const renderer;
};