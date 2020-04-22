#include "ImportHelper.h"
#include "FileImporter.h"

#include "../Graphics/Texture.h"
#include "../Graphics/Model.h"
#include "../Graphics/Shaders.h"
#include "../../Util/ErrorLogger.h"
#include "ObjectPool.h"
#include <io.h>
#include <WICTextureLoader.h>
#include <DDSTextureLoader.h>
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <DirectXMath.h>
#include <fstream>
#include <sstream>

void Importer::LoadData()
{
	TraverseDirectory("hlsl/VertexShader/2D/", &Importer::LoadVertexShader2D);
	TraverseDirectory("hlsl/VertexShader/3D/", &Importer::LoadVertexShader3D);
	TraverseDirectory("hlsl/VertexShader/3D_Skinned/", &Importer::LoadVertexShader);
	TraverseDirectory("hlsl/PixelShader/", &Importer::LoadPixelShader);
	TraverseDirectory("Data/Textures/", &Importer::LoadTexture); 
	TraverseDirectory("Data/Objects/", &Importer::LoadModel);
	TraverseDirectory("Data/Animation/", &Importer::LoadAnimation);
}

bool CreateModel(const std::string & dirPath, const std::string& name) {
	auto filePath = dirPath + name;
	Assimp::Importer importer;
	const aiScene* pScene = importer.ReadFile(filePath,
		aiProcess_Triangulate |
		aiProcess_ConvertToLeftHanded);

	if (pScene == nullptr) return false;

	if (pScene->HasMeshes()) {
		if (!pScene->HasAnimations()) {
			ModelImporter importer;
			return importer.CreateModel(dirPath, StringHelper::EraseFileExtension(name), pScene);
		}
		else {
			SkinnedModelImporter importer;
			return importer.CreateModel(dirPath, StringHelper::EraseFileExtension(name), pScene);
		}
	}

	return false;
}
void Importer::LoadModel(const std::string & dirPath, const std::string& name)
{
	std::string ext = StringHelper::GetFileExtension(name);
	if (ext == "fbx" ||
		ext == "obj")
	{
		if (!CreateModel(dirPath, name)) {
			MessageBoxA(NULL, "Model Initialize error.", ERROR, MB_ICONERROR);
			return;
		}
	}
	
}

bool CreateAnimation(const std::string & dirPath, const std::string& name) {
	auto filePath = dirPath + name;
	Assimp::Importer importer;
	const aiScene* pScene = importer.ReadFile(filePath,
		aiProcess_Triangulate |
		aiProcess_ConvertToLeftHanded);

	if (pScene == nullptr) return false;

	if (pScene->HasAnimations()) {
		AnimationImporter animImporter;
		auto avatarName = StringHelper::GetUpperDirectroyName(filePath);

		animImporter.LoadAnimation(
			avatarName + "_" + StringHelper::EraseFileExtension(name),
			Pool::Find<SkinnedModel>(avatarName), pScene);
	}

	return true;
}
void Importer::LoadAnimation(const std::string & dirPath, const std::string& name)
{
	std::string ext = StringHelper::GetFileExtension(name);
	if (ext == "fbx")
	{
		if (!CreateAnimation(dirPath, name)) {
			MessageBoxA(NULL, "Animation Initialize error.", ERROR, MB_ICONERROR);
			return;
		}
	}
	
}

void Importer::LoadVertexShader2D(const std::string & dirPath, const std::string& name)
{
	std::string ext = StringHelper::GetFileExtension(name);
	if (ext == "hlsl")
	{
		D3D11_INPUT_ELEMENT_DESC layout[] = {
			{"POSITION", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA, 0},
			{"TEXCOORD", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA}
		};

		auto vs = Pool::CreateInstance<VertexShader>();
		if (!vs->Initialize(
			Core::GetBuildPath() + StringHelper::EraseFileExtension(name) + ".cso",
			layout,
			ARRAYSIZE(layout)))
		{
			MessageBoxA(NULL, "Shader Initialize error.", ERROR, MB_ICONERROR);
		}
	}
}

void Importer::LoadVertexShader3D(const std::string & dirPath, const std::string& name)
{
	std::string ext = StringHelper::GetFileExtension(name);
	if (ext == "hlsl")
	{
		D3D11_INPUT_ELEMENT_DESC layout[] = {
			{"POSITION", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA, 0},
			{"TEXCOORD", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA},
			{"NORMAL", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA}
		};

		auto vs = Pool::CreateInstance<VertexShader>();
		if (!vs->Initialize(
			Core::GetBuildPath() + StringHelper::EraseFileExtension(name) + ".cso",
			layout,
			ARRAYSIZE(layout)))
		{
			MessageBoxA(NULL, "Shader Initialize error.", ERROR, MB_ICONERROR);
		}
	}
}

void Importer::LoadVertexShader(const std::string & dirPath, const std::string& name)
{
	std::string ext = StringHelper::GetFileExtension(name);
	if (ext == "hlsl")
	{
		D3D11_INPUT_ELEMENT_DESC layout[] = {
			{"POSITION", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA, 0},
			{"TEXCOORD", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA},
			{"NORMAL", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA},
			{"BONEID", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32B32A32_SINT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA},
			{"BONEWEIGHT", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA}
		};

		auto vs = Pool::CreateInstance<VertexShader>();
		if (!vs->Initialize(
			Core::GetBuildPath() + StringHelper::EraseFileExtension(name) + ".cso",
			layout,
			ARRAYSIZE(layout)))
		{
			Pool::Destroy(vs);
			MessageBoxA(NULL, "Shader Initialize error.", ERROR, MB_ICONERROR);
		}
	}
}

void Importer::LoadPixelShader(const std::string & dirPath, const std::string& name)
{
	std::string ext = StringHelper::GetFileExtension(name);
	if (ext == "hlsl")
	{
		auto ps = Pool::CreateInstance<PixelShader>();
		if (!ps->Initialize(Core::GetBuildPath() + StringHelper::EraseFileExtension(name) + ".cso"))
		{
			MessageBoxA(NULL, "Shader Initialize error.", ERROR, MB_ICONERROR);
		}
	}
}

void Importer::LoadTexture(const std::string & dirPath, const std::string& name)
{
	std::string ext = StringHelper::GetFileExtension(name);
	if (ext == "jpg" ||
		ext == "png" ||
		ext == "bmp")
	{
		Pool::CreateInstance<Texture>(dirPath + name, aiTextureType::aiTextureType_DIFFUSE);
	}
}

void Importer::LoadCSV(const std::string & dirPath, const std::string & name)
{
	std::string ext = StringHelper::GetFileExtension(name);
	if (ext == "csv")
	{

	}
}

std::unordered_map<std::string, std::vector<std::string>> Importer::LoadCSV(const std::string & filePath)
{
	CsvImporter importer;
	return importer.LoadCSV(filePath);
}

std::vector<std::string> Importer::SplitString(const std::string & str, char delimiter)
{
	std::vector<std::string> vs;
	std::string word;

	std::stringstream stream(str);

	while (std::getline(stream, word, delimiter)) vs.push_back(word);
	return vs;
}

void Importer::TraverseDirectory(const std::string & dirPath, void(*callBack)(const std::string &, const std::string &))
{
	struct _finddata_t fd;
	intptr_t handle;

	std::string filePath = dirPath + "*.*";

	if ((handle = _findfirst(filePath.c_str(), &fd)) != -1L) {
		do {
			if (fd.attrib & _A_SUBDIR &&
				(fd.name != std::string(".")) &&
				(fd.name != std::string("..")))
			{
				std::string subDirPath = dirPath + fd.name + "/";
				TraverseDirectory(subDirPath, callBack);
				continue;
			}

			(*callBack)(dirPath, fd.name);
		} while (_findnext(handle, &fd) == 0);
	}
	_findclose(handle);
}
