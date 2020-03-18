#include "Terrain.h"
#define TERRAIN_MUL 1

Terrain::~Terrain()
{
	
}

Terrain::Terrain(const COMPONENT_INIT_DESC & desc) : Component(desc), transform(desc.mTransform), renderer(desc.mRenderer) { std::strcpy(mComponentName, "Terrain"); }

TERRAIN_INIT_DESC Terrain::TerrainProcess(const char * HeightMapPath)
{
	if (!LoadHeightMap(HeightMapPath)) {
		assert("Load Height Map Failed." && 1 == 0);
		//return false;
	}

	NormalizeHeightMap();

	if (!CalculateNormals()) {
		assert("Terrain Normal Calculate Failed." && 1 == 0);
		//return false;
	}
	
	CalculateTextureCoordinates();

	return CreateTerrainModel();
}

void Terrain::Shutdown()
{
}

void Terrain::Redner()
{
}

bool Terrain::LoadHeightMap(const char * filename)
{
	FILE* filePtr;
	int error;
	unsigned int count;
	BITMAPFILEHEADER bitmapFileHeader;
	BITMAPINFOHEADER bitmapInfoHeader;
	int imageSize, i, j, k = 0, index;
	unsigned char* bitmapImage;
	unsigned char height;

		// Open the height map file in binary.
		error = fopen_s(&filePtr, filename, "rb");
	if (error != 0)
	{
		return false;
	}

	// Read in the file header.
	count = fread(&bitmapFileHeader, sizeof(BITMAPFILEHEADER), (size_t)1, filePtr);
	if (count != 1)
	{
		return false;
	}

	// Read in the bitmap info header.
	count = fread(&bitmapInfoHeader, sizeof(BITMAPINFOHEADER), (size_t)1, filePtr);
	if (count != 1)
	{
		return false;
	}

	// Save the dimensions of the terrain.
	m_terrainWidth = bitmapInfoHeader.biWidth;
	m_terrainHeight = bitmapInfoHeader.biHeight;

	// Calculate the size of the bitmap image data.
	imageSize = m_terrainWidth * m_terrainHeight * 3;

	// Allocate memory for the bitmap image data.
	bitmapImage = new unsigned char[imageSize];
	if (!bitmapImage)
	{
		return false;
	}

	// Move to the beginning of the bitmap data.
	fseek(filePtr, bitmapFileHeader.bfOffBits, SEEK_SET);

	// Read in the bitmap image data.
	count = fread(bitmapImage, (size_t)1, (size_t)imageSize, filePtr);
	if (count != imageSize)
	{
		return false;
	}

	// Close the file.
	error = fclose(filePtr);
	if (error != 0)
	{
		return false;
	}

	//이미지 데이터 버퍼의 위치를 초기화
	m_heightMap = new HeightMapType[m_terrainWidth * m_terrainHeight];

	//이미지 데이터를 높이 맵으로 읽기
	for (j = 0; j < m_terrainHeight; j++) {
		for (i = 0; i < m_terrainWidth; i++) {
			height = bitmapImage[k];

			index = (m_terrainHeight * j) + i;
			//unsigned char height = in[index];

			m_heightMap[index].x = (float)i * TERRAIN_MUL;
			m_heightMap[index].y = (float)height * TERRAIN_MUL;
			m_heightMap[index].z = (float)j * TERRAIN_MUL;

			k += 3;
		}
	}

	//비트맵 이미지 데이터 해체
	delete[] bitmapImage;
	bitmapImage = 0;

	return true;
}

void Terrain::NormalizeHeightMap()
{
	for (int j = 0; j < m_terrainHeight; j++) {
		for (int i = 0; i < m_terrainWidth; i++) {
			m_heightMap[(m_terrainHeight * j) + i].y /= 15.0f;
		}
	}
}

bool Terrain::CalculateNormals()
{
	int index1 = 0;
	int index2 = 0;
	int index3 = 0;
	int index = 0;
	int count = 0;
	float vertex1[3] = { 0.f, 0.f, 0.f };
	float vertex2[3] = { 0.f, 0.f, 0.f };
	float vertex3[3] = { 0.f, 0.f, 0.f };
	float vector1[3] = { 0.f, 0.f, 0.f };
	float vector2[3] = { 0.f, 0.f, 0.f };
	float sum[3] = { 0.f, 0.f, 0.f };
	float length = 0.0f;


	// 정규화되지 않은 법선 벡터를 저장할 임시 배열을 만듭니다.
	VectorType* normals = new VectorType[(m_terrainHeight - 1) * (m_terrainWidth - 1)];
	if (!normals)
	{
		return false;
	}

	// 메쉬의 모든면을 살펴보고 법선을 계산합니다.
	for (int j = 0; j < (m_terrainHeight - 1); j++)
	{
		for (int i = 0; i < (m_terrainWidth - 1); i++)
		{
			index1 = (j * m_terrainHeight) + i;
			index2 = (j * m_terrainHeight) + (i + 1);
			index3 = ((j + 1) * m_terrainHeight) + i;

			// 표면에서 세 개의 꼭지점을 가져옵니다.
			vertex1[0] = m_heightMap[index1].x;
			vertex1[1] = m_heightMap[index1].y;
			vertex1[2] = m_heightMap[index1].z;

			vertex2[0] = m_heightMap[index2].x;
			vertex2[1] = m_heightMap[index2].y;
			vertex2[2] = m_heightMap[index2].z;

			vertex3[0] = m_heightMap[index3].x;
			vertex3[1] = m_heightMap[index3].y;
			vertex3[2] = m_heightMap[index3].z;

			// 표면의 두 벡터를 계산합니다.
			vector1[0] = vertex1[0] - vertex3[0];
			vector1[1] = vertex1[1] - vertex3[1];
			vector1[2] = vertex1[2] - vertex3[2];
			vector2[0] = vertex3[0] - vertex2[0];
			vector2[1] = vertex3[1] - vertex2[1];
			vector2[2] = vertex3[2] - vertex2[2];

			index = (j * (m_terrainHeight - 1)) + i;

			// 이 두 법선에 대한 정규화되지 않은 값을 얻기 위해 두 벡터의 외적을 계산합니다.
			normals[index].x = (vector1[1] * vector2[2]) - (vector1[2] * vector2[1]);
			normals[index].y = (vector1[2] * vector2[0]) - (vector1[0] * vector2[2]);
			normals[index].z = (vector1[0] * vector2[1]) - (vector1[1] * vector2[0]);
		}
	}

	// 이제 모든 정점을 살펴보고 각면의 평균을 취합니다. 	
	// 정점이 닿아 그 정점에 대한 평균 평균값을 얻는다.
	for (int j = 0; j < m_terrainHeight; j++)
	{
		for (int i = 0; i < m_terrainWidth; i++)
		{
			// 합계를 초기화합니다.
			sum[0] = 0.0f;
			sum[1] = 0.0f;
			sum[2] = 0.0f;

			// 카운트를 초기화합니다.
			count = 0;

			// 왼쪽 아래면.
			if (((i - 1) >= 0) && ((j - 1) >= 0))
			{
				index = ((j - 1) * (m_terrainHeight - 1)) + (i - 1);

				sum[0] += normals[index].x;
				sum[1] += normals[index].y;
				sum[2] += normals[index].z;
				count++;
			}

			// 오른쪽 아래 면.
			if ((i < (m_terrainWidth - 1)) && ((j - 1) >= 0))
			{
				index = ((j - 1) * (m_terrainHeight - 1)) + i;

				sum[0] += normals[index].x;
				sum[1] += normals[index].y;
				sum[2] += normals[index].z;
				count++;
			}

			// 왼쪽 위 면.
			if (((i - 1) >= 0) && (j < (m_terrainHeight - 1)))
			{
				index = (j * (m_terrainHeight - 1)) + (i - 1);

				sum[0] += normals[index].x;
				sum[1] += normals[index].y;
				sum[2] += normals[index].z;
				count++;
			}

			// 오른쪽 위 면.
			if ((i < (m_terrainWidth - 1)) && (j < (m_terrainHeight - 1)))
			{
				index = (j * (m_terrainHeight - 1)) + i;

				sum[0] += normals[index].x;
				sum[1] += normals[index].y;
				sum[2] += normals[index].z;
				count++;
			}

			// 이 정점에 닿는면의 평균을 취합니다.
			sum[0] = (sum[0] / (float)count);
			sum[1] = (sum[1] / (float)count);
			sum[2] = (sum[2] / (float)count);

			// 이 법선의 길이를 계산합니다.
			length = (float)sqrt((sum[0] * sum[0]) + (sum[1] * sum[1]) + (sum[2] * sum[2]));

			// 높이 맵 배열의 정점 위치에 대한 인덱스를 가져옵니다.
			index = (j * m_terrainHeight) + i;

			// 이 정점의 최종 공유 법선을 표준화하여 높이 맵 배열에 저장합니다.
			m_heightMap[index].nx = (sum[0] / length);
			m_heightMap[index].ny = (sum[1] / length);
			m_heightMap[index].nz = (sum[2] / length);
		}
	}

	// 임시 법선을 해제합니다.
	delete[] normals;
	normals = 0;

	return true;
}

void Terrain::CalculateTextureCoordinates()
{
	// 텍스처 좌표를 얼마나 많이 증가 시킬지 계산합니다.
	float incrementValue = (float)Texture_Repeat_Count / (float)m_terrainWidth;

	// 텍스처를 반복 할 횟수를 계산합니다.
	int incrementCount = m_terrainWidth / Texture_Repeat_Count;

	// tu 및 tv 좌표 값을 초기화합니다.
	float tuCoordinate = 0.0f;
	float tvCoordinate = 1.0f;

	//  tu 및 tv 좌표 인덱스를 초기화합니다.
	int tuCount = 0;
	int tvCount = 0;

	// 전체 높이 맵을 반복하고 각 꼭지점의 tu 및 tv 텍스처 좌표를 계산합니다.
	for (int j = 0; j < m_terrainHeight; j++)
	{
		for (int i = 0; i < m_terrainWidth; i++)
		{
			// 높이 맵에 텍스처 좌표를 저장한다.
			m_heightMap[(m_terrainHeight * j) + i].tu = tuCoordinate;
			m_heightMap[(m_terrainHeight * j) + i].tv = tvCoordinate;

			// tu 텍스처 좌표를 증가 값만큼 증가시키고 인덱스를 1 씩 증가시킨다.
			tuCoordinate += incrementValue;
			tuCount++;

			// 텍스처의 오른쪽 끝에 있는지 확인하고, 그렇다면 처음부터 다시 시작하십시오.
			if (tuCount == incrementCount)
			{
				tuCoordinate = 0.0f;
				tuCount = 0;
			}
		}

		// tv 텍스처 좌표를 증가 값만큼 증가시키고 인덱스를 1 씩 증가시킵니다.
		tvCoordinate -= incrementValue;
		tvCount++;

		// 텍스처의 상단에 있는지 확인하고, 그렇다면 하단에서 다시 시작합니다.
		if (tvCount == incrementCount)
		{
			tvCoordinate = 1.0f;
			tvCount = 0;
		}
	}
}

TERRAIN_INIT_DESC Terrain::CreateTerrainModel()
{
	int size = (m_terrainHeight - 1) * (m_terrainWidth - 1) * 6;

	std::vector<Vertex3D> vertexBuffer(size);
	std::vector<DWORD> indexBuffer(size);

	float tu = 0.0f;
	float tv = 0.0f;

	int index = 0;
	
	// 지형 데이터로 정점 및 인덱스 배열 생성
	for (int j = 0; j < (m_terrainHeight - 1); j++)
	{
		for (int i = 0; i < (m_terrainWidth - 1); i++)
		{
			int index1 = (m_terrainHeight * j) + i;          // 왼쪽 아래.
			int index2 = (m_terrainHeight * j) + (i + 1);      // 오른쪽 아래.
			int index3 = (m_terrainHeight * (j + 1)) + i;      // 왼쪽 위.
			int index4 = (m_terrainHeight * (j + 1)) + (i + 1);  // 오른쪽 위.

			// 왼쪽 위.
			tv = m_heightMap[index3].tv;

			// 상단 가장자리를 덮도록 텍스처 좌표를 수정합니다.
			if (tv == 1.0f) { tv = 0.0f; }

			vertexBuffer[index] = Vertex3D(
					m_heightMap[index3].x, m_heightMap[index3].y, m_heightMap[index3].z,
					m_heightMap[index3].tu, tv,
					m_heightMap[index3].nx, m_heightMap[index3].ny, m_heightMap[index3].nz);
			indexBuffer[index] = index++;

			// 오른쪽 위.
			tu = m_heightMap[index4].tu;
			tv = m_heightMap[index4].tv;

			// 위쪽과 오른쪽 가장자리를 덮도록 텍스처 좌표를 수정합니다.
			if (tu == 0.0f) { tu = 1.0f; }
			if (tv == 1.0f) { tv = 0.0f; }

			vertexBuffer[index] = Vertex3D(
				m_heightMap[index4].x, m_heightMap[index4].y, m_heightMap[index4].z,
				tu, tv,
				m_heightMap[index4].nx, m_heightMap[index4].ny, m_heightMap[index4].nz);
				indexBuffer[index] = index++;

			vertexBuffer[index] = Vertex3D(
				m_heightMap[index1].x, m_heightMap[index1].y, m_heightMap[index1].z,
				m_heightMap[index1].tu, m_heightMap[index1].tv,
				m_heightMap[index1].nx, m_heightMap[index1].ny, m_heightMap[index1].nz);
				indexBuffer[index] = index++;

			vertexBuffer[index] = Vertex3D(
					m_heightMap[index1].x, m_heightMap[index1].y, m_heightMap[index1].z,
					m_heightMap[index1].tu, m_heightMap[index1].tv,
					m_heightMap[index1].nx, m_heightMap[index1].ny, m_heightMap[index1].nz);
			indexBuffer[index] = index++;


			// 오른쪽 위.
			tu = m_heightMap[index4].tu;
			tv = m_heightMap[index4].tv;

			// 위쪽과 오른쪽 가장자리를 덮도록 텍스처 좌표를 수정합니다.
			if (tu == 0.0f) { tu = 1.0f; }
			if (tv == 1.0f) { tv = 0.0f; }

			vertexBuffer[index] = Vertex3D(
				m_heightMap[index4].x, m_heightMap[index4].y, m_heightMap[index4].z,
				tu, tv,
				m_heightMap[index4].nx, m_heightMap[index4].ny, m_heightMap[index4].nz);
			indexBuffer[index] = index++;

			// 오른쪽 아래.
			tu = m_heightMap[index2].tu;

			// 오른쪽 가장자리를 덮도록 텍스처 좌표를 수정합니다.
			if (tu == 0.0f) { tu = 1.0f; }

			vertexBuffer[index] = Vertex3D(
				m_heightMap[index2].x, m_heightMap[index2].y, m_heightMap[index2].z,
				m_heightMap[index2].tu, tv,
				m_heightMap[index2].nx, m_heightMap[index2].ny, m_heightMap[index2].nz);
			indexBuffer[index] = index++;
		}
	}
	
	delete[] m_heightMap;
	m_heightMap = nullptr;

	TERRAIN_INIT_DESC desc = { vertexBuffer, indexBuffer };

	return desc;
}

int Terrain::GetIndexCount()
{
	return m_indexCount;
}
