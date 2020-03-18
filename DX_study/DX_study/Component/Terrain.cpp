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

	//�̹��� ������ ������ ��ġ�� �ʱ�ȭ
	m_heightMap = new HeightMapType[m_terrainWidth * m_terrainHeight];

	//�̹��� �����͸� ���� ������ �б�
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

	//��Ʈ�� �̹��� ������ ��ü
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


	// ����ȭ���� ���� ���� ���͸� ������ �ӽ� �迭�� ����ϴ�.
	VectorType* normals = new VectorType[(m_terrainHeight - 1) * (m_terrainWidth - 1)];
	if (!normals)
	{
		return false;
	}

	// �޽��� ������ ���캸�� ������ ����մϴ�.
	for (int j = 0; j < (m_terrainHeight - 1); j++)
	{
		for (int i = 0; i < (m_terrainWidth - 1); i++)
		{
			index1 = (j * m_terrainHeight) + i;
			index2 = (j * m_terrainHeight) + (i + 1);
			index3 = ((j + 1) * m_terrainHeight) + i;

			// ǥ�鿡�� �� ���� �������� �����ɴϴ�.
			vertex1[0] = m_heightMap[index1].x;
			vertex1[1] = m_heightMap[index1].y;
			vertex1[2] = m_heightMap[index1].z;

			vertex2[0] = m_heightMap[index2].x;
			vertex2[1] = m_heightMap[index2].y;
			vertex2[2] = m_heightMap[index2].z;

			vertex3[0] = m_heightMap[index3].x;
			vertex3[1] = m_heightMap[index3].y;
			vertex3[2] = m_heightMap[index3].z;

			// ǥ���� �� ���͸� ����մϴ�.
			vector1[0] = vertex1[0] - vertex3[0];
			vector1[1] = vertex1[1] - vertex3[1];
			vector1[2] = vertex1[2] - vertex3[2];
			vector2[0] = vertex3[0] - vertex2[0];
			vector2[1] = vertex3[1] - vertex2[1];
			vector2[2] = vertex3[2] - vertex2[2];

			index = (j * (m_terrainHeight - 1)) + i;

			// �� �� ������ ���� ����ȭ���� ���� ���� ��� ���� �� ������ ������ ����մϴ�.
			normals[index].x = (vector1[1] * vector2[2]) - (vector1[2] * vector2[1]);
			normals[index].y = (vector1[2] * vector2[0]) - (vector1[0] * vector2[2]);
			normals[index].z = (vector1[0] * vector2[1]) - (vector1[1] * vector2[0]);
		}
	}

	// ���� ��� ������ ���캸�� ������ ����� ���մϴ�. 	
	// ������ ��� �� ������ ���� ��� ��հ��� ��´�.
	for (int j = 0; j < m_terrainHeight; j++)
	{
		for (int i = 0; i < m_terrainWidth; i++)
		{
			// �հ踦 �ʱ�ȭ�մϴ�.
			sum[0] = 0.0f;
			sum[1] = 0.0f;
			sum[2] = 0.0f;

			// ī��Ʈ�� �ʱ�ȭ�մϴ�.
			count = 0;

			// ���� �Ʒ���.
			if (((i - 1) >= 0) && ((j - 1) >= 0))
			{
				index = ((j - 1) * (m_terrainHeight - 1)) + (i - 1);

				sum[0] += normals[index].x;
				sum[1] += normals[index].y;
				sum[2] += normals[index].z;
				count++;
			}

			// ������ �Ʒ� ��.
			if ((i < (m_terrainWidth - 1)) && ((j - 1) >= 0))
			{
				index = ((j - 1) * (m_terrainHeight - 1)) + i;

				sum[0] += normals[index].x;
				sum[1] += normals[index].y;
				sum[2] += normals[index].z;
				count++;
			}

			// ���� �� ��.
			if (((i - 1) >= 0) && (j < (m_terrainHeight - 1)))
			{
				index = (j * (m_terrainHeight - 1)) + (i - 1);

				sum[0] += normals[index].x;
				sum[1] += normals[index].y;
				sum[2] += normals[index].z;
				count++;
			}

			// ������ �� ��.
			if ((i < (m_terrainWidth - 1)) && (j < (m_terrainHeight - 1)))
			{
				index = (j * (m_terrainHeight - 1)) + i;

				sum[0] += normals[index].x;
				sum[1] += normals[index].y;
				sum[2] += normals[index].z;
				count++;
			}

			// �� ������ ��¸��� ����� ���մϴ�.
			sum[0] = (sum[0] / (float)count);
			sum[1] = (sum[1] / (float)count);
			sum[2] = (sum[2] / (float)count);

			// �� ������ ���̸� ����մϴ�.
			length = (float)sqrt((sum[0] * sum[0]) + (sum[1] * sum[1]) + (sum[2] * sum[2]));

			// ���� �� �迭�� ���� ��ġ�� ���� �ε����� �����ɴϴ�.
			index = (j * m_terrainHeight) + i;

			// �� ������ ���� ���� ������ ǥ��ȭ�Ͽ� ���� �� �迭�� �����մϴ�.
			m_heightMap[index].nx = (sum[0] / length);
			m_heightMap[index].ny = (sum[1] / length);
			m_heightMap[index].nz = (sum[2] / length);
		}
	}

	// �ӽ� ������ �����մϴ�.
	delete[] normals;
	normals = 0;

	return true;
}

void Terrain::CalculateTextureCoordinates()
{
	// �ؽ�ó ��ǥ�� �󸶳� ���� ���� ��ų�� ����մϴ�.
	float incrementValue = (float)Texture_Repeat_Count / (float)m_terrainWidth;

	// �ؽ�ó�� �ݺ� �� Ƚ���� ����մϴ�.
	int incrementCount = m_terrainWidth / Texture_Repeat_Count;

	// tu �� tv ��ǥ ���� �ʱ�ȭ�մϴ�.
	float tuCoordinate = 0.0f;
	float tvCoordinate = 1.0f;

	//  tu �� tv ��ǥ �ε����� �ʱ�ȭ�մϴ�.
	int tuCount = 0;
	int tvCount = 0;

	// ��ü ���� ���� �ݺ��ϰ� �� �������� tu �� tv �ؽ�ó ��ǥ�� ����մϴ�.
	for (int j = 0; j < m_terrainHeight; j++)
	{
		for (int i = 0; i < m_terrainWidth; i++)
		{
			// ���� �ʿ� �ؽ�ó ��ǥ�� �����Ѵ�.
			m_heightMap[(m_terrainHeight * j) + i].tu = tuCoordinate;
			m_heightMap[(m_terrainHeight * j) + i].tv = tvCoordinate;

			// tu �ؽ�ó ��ǥ�� ���� ����ŭ ������Ű�� �ε����� 1 �� ������Ų��.
			tuCoordinate += incrementValue;
			tuCount++;

			// �ؽ�ó�� ������ ���� �ִ��� Ȯ���ϰ�, �׷��ٸ� ó������ �ٽ� �����Ͻʽÿ�.
			if (tuCount == incrementCount)
			{
				tuCoordinate = 0.0f;
				tuCount = 0;
			}
		}

		// tv �ؽ�ó ��ǥ�� ���� ����ŭ ������Ű�� �ε����� 1 �� ������ŵ�ϴ�.
		tvCoordinate -= incrementValue;
		tvCount++;

		// �ؽ�ó�� ��ܿ� �ִ��� Ȯ���ϰ�, �׷��ٸ� �ϴܿ��� �ٽ� �����մϴ�.
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
	
	// ���� �����ͷ� ���� �� �ε��� �迭 ����
	for (int j = 0; j < (m_terrainHeight - 1); j++)
	{
		for (int i = 0; i < (m_terrainWidth - 1); i++)
		{
			int index1 = (m_terrainHeight * j) + i;          // ���� �Ʒ�.
			int index2 = (m_terrainHeight * j) + (i + 1);      // ������ �Ʒ�.
			int index3 = (m_terrainHeight * (j + 1)) + i;      // ���� ��.
			int index4 = (m_terrainHeight * (j + 1)) + (i + 1);  // ������ ��.

			// ���� ��.
			tv = m_heightMap[index3].tv;

			// ��� �����ڸ��� ������ �ؽ�ó ��ǥ�� �����մϴ�.
			if (tv == 1.0f) { tv = 0.0f; }

			vertexBuffer[index] = Vertex3D(
					m_heightMap[index3].x, m_heightMap[index3].y, m_heightMap[index3].z,
					m_heightMap[index3].tu, tv,
					m_heightMap[index3].nx, m_heightMap[index3].ny, m_heightMap[index3].nz);
			indexBuffer[index] = index++;

			// ������ ��.
			tu = m_heightMap[index4].tu;
			tv = m_heightMap[index4].tv;

			// ���ʰ� ������ �����ڸ��� ������ �ؽ�ó ��ǥ�� �����մϴ�.
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


			// ������ ��.
			tu = m_heightMap[index4].tu;
			tv = m_heightMap[index4].tv;

			// ���ʰ� ������ �����ڸ��� ������ �ؽ�ó ��ǥ�� �����մϴ�.
			if (tu == 0.0f) { tu = 1.0f; }
			if (tv == 1.0f) { tv = 0.0f; }

			vertexBuffer[index] = Vertex3D(
				m_heightMap[index4].x, m_heightMap[index4].y, m_heightMap[index4].z,
				tu, tv,
				m_heightMap[index4].nx, m_heightMap[index4].ny, m_heightMap[index4].nz);
			indexBuffer[index] = index++;

			// ������ �Ʒ�.
			tu = m_heightMap[index2].tu;

			// ������ �����ڸ��� ������ �ؽ�ó ��ǥ�� �����մϴ�.
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
